// Fill out your copyright notice in the Description page of Project Settings.


#include "Both/CharacterAnimation.h"
#include "Code/Actors/BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Code/Actors/Agent.h"
//#include "CharacterAnimation.h"

UCharacterAnimation::UCharacterAnimation()
{
	ActionSlotName = FName("ActionSlot");
	Velocity = 0.f;
	Direction = 0.f;
}

void UCharacterAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningPawn)
		OwningPawn = TryGetPawnOwner();
	if (!OwningPawn)
		return;

	ACharacter* Character = Cast<ACharacter>(OwningPawn);
	if (!Character)
		return;

	const UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
		return;

	FVector Vel = Character->GetVelocity();
	Vel.Z = 0.f;

	Velocity = Vel.Size();

	// Set this BEFORE using it
	IsInAir = MoveComp->IsFalling();

	
	ABasePlayer* Player = Cast<ABasePlayer>(Character);

	if (!Player) // this means it is a zombie or AI
	{
		ZombieSpeedRatio = Velocity / 150.f;
		ZombieSpeedRatio = FMath::Clamp(ZombieSpeedRatio, 0.5f, 1.5f);
	}

	if (!Player)
	{
		AAgent* Agent = Cast<AAgent>(Character);
		if (Agent)
		{
			bIsZombieAttacking = Agent->bIsAttacking;
		}
		else
		{
			bIsZombieAttacking = false;
		}
	}


	if (Player)
	{
		const bool bJustLanded = (bWasInAirPrev && !IsInAir);

		if (bJustLanded && LandSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				LandSound,
				Character->GetActorLocation()
			);
		}
	}

	// update after checks
	bWasInAirPrev = IsInAir;

	// --------------------
	// STOP CUE (player only)
	// --------------------
	if (Player)
	{
		const float Speed2D = Velocity;

		const bool bWasMoving = PrevSpeed2D > WasMovingThreshold;
		const bool bStoppedNow = Speed2D <= StopSpeedThreshold;

		if (!IsInAir && bWasMoving && bStoppedNow && FootstepStopSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				FootstepStopSound,
				Character->GetActorLocation()
			);
		}

		PrevSpeed2D = Speed2D;
	}


	Direction = UKismetAnimationLibrary::CalculateDirection(Vel, Character->GetActorRotation());

	// --------------------
	// JUMP CUE (player only)
	// --------------------
	if (Player && Player->bJustJumped)
	{
		// your existing animation trigger
		JumpAlpha = 1.f;
		bJustJumped = true;

		// play the jump cue once
		if (JumpSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				JumpSound,
				Character->GetActorLocation()
			);
		}

		// consume the flag so it doesn't spam
		Player->bJustJumped = false;
	}
	else
	{
		bJustJumped = false;
	}

	// Equip state (player only)
	if (Player)
	{
		EquipState = Player->EquipState;
	}
}

void UCharacterAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();

	Velocity = 0.f;
	Direction = 0.f;
	IsInAir = false;
	JumpAlpha = 0.f;
	bJustJumped = false;
	bIsDead = false;
	bShouldFire = false;
	bIsZombieAttacking = false;
	EquipState = EEquipState::Unarmed;
}


void UCharacterAnimation::SetFireAsset(UAnimSequence* InAnim)
{
	if (!InAnim || !IsValid(InAnim))
	{
		return;
	}

	FireAsset = InAnim;
}

void UCharacterAnimation::FireAnimation()
{
	PlaySlotAnimationAsDynamicMontage(FireAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
}

void UCharacterAnimation::HitAnimation()
{
	if (HurtSound && TryGetPawnOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HurtSound,
			TryGetPawnOwner()->GetActorLocation()
		);
	}

	PlaySlotAnimationAsDynamicMontage(HurtAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
}

void UCharacterAnimation::PlayDeathAnimation()
{
	SelectDeathAsset();

	bIsDead = true; // <-- THIS drives the state machine

	const float Duration = CurrentDeathAsset->GetPlayLength();

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &UCharacterAnimation::DeathEnded, Duration, false);
	}
}


void UCharacterAnimation::PreviewWindowUpdate_Implementation()
{
	Velocity = 0.f;
	Direction = 0.f;
}

void UCharacterAnimation::SelectDeathAsset()
{
	if (DeathAssets.Num() == 0)
	{
		return;
	}

	int32 Index = FMath::RandRange(0, DeathAssets.Num() - 1);
	CurrentDeathAsset = DeathAssets[Index];

}

void UCharacterAnimation::ReloadAnimation()
{

	PlaySlotAnimationAsDynamicMontage(ReloadAsset, ActionSlotName, 0.25f, 0.25f, 1.0f, 1);
}

void UCharacterAnimation::DeathEnded()
{
	OnDeathEnded.Broadcast();
}

void UCharacterAnimation::AxeAnimation()
{
	PlaySlotAnimationAsDynamicMontage(AxeAsset, ActionSlotName, 0.25f, 0.25f, 1.0f, 1);
}

void UCharacterAnimation::PickaxeAnimation()
{

	PlaySlotAnimationAsDynamicMontage(PickaxeAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
}

void UCharacterAnimation::FlashlightAnimation()
{
	PlaySlotAnimationAsDynamicMontage(
		FlashlightAsset,
		ActionSlotName,   // must match slot node name in AnimBP
		0.1f, 0.1f, 1.0f, 1);

}

void UCharacterAnimation::AnimNotify_Footstep()
{
	APawn* Pawn = TryGetPawnOwner();
	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!Character) return;

	ABasePlayer* Player = Cast<ABasePlayer>(Pawn);
	if (!Player) return;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return;

	const float Speed2D = Character->GetVelocity().Size2D();
	const bool bFalling = MoveComp->IsFalling();

	// Gate
	if (!MoveComp->IsMovingOnGround()) return;
	if (Speed2D <= MinFootstepSpeed) return;

	if (!FootstepSound) return;

	UGameplayStatics::PlaySoundAtLocation(Character, FootstepSound, Character->GetActorLocation());
}

void UCharacterAnimation::ZombieAttackAnimation()
{
	PlaySlotAnimationAsDynamicMontage(ZombieAttackAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);

}