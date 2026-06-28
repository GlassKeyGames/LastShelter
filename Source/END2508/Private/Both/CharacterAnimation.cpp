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
		UE_LOG(LogTemp, Error, TEXT("Invalid FireAsset passed to SetFireAsset"));
		return;
	}

	FireAsset = InAnim;
	UE_LOG(LogTemp, Warning, TEXT("SetFireAsset assigned: %s"), *InAnim->GetName());
}

void UCharacterAnimation::FireAnimation()
{
	UE_LOG(LogTemp, Error, TEXT("=== FireAnimation CALLED | Pawn=%s | Time=%.3f ==="),
		*GetNameSafe(TryGetPawnOwner()),
		GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f);

	if (!FireAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("FireAsset is null"));
		return;
	}

	PlaySlotAnimationAsDynamicMontage(FireAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
	UE_LOG(LogTemp, Warning, TEXT("Fire animation played: %s"), *FireAsset->GetName());
}

void UCharacterAnimation::HitAnimation()
{
	if (!HurtAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("HurtAsset is null"));
		return;
	}

	if (HurtSound && TryGetPawnOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HurtSound,
			TryGetPawnOwner()->GetActorLocation()
		);
	}

	UE_LOG(LogTemp, Error, TEXT("=== HitAnimation CALLED | Pawn=%s | Time=%.3f ==="),
		*GetNameSafe(TryGetPawnOwner()),
		GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f);

	PlaySlotAnimationAsDynamicMontage(HurtAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
	UE_LOG(LogTemp, Warning, TEXT("Hurt animation played: %s"), *HurtAsset->GetName());
}

void UCharacterAnimation::PlayDeathAnimation()
{
	SelectDeathAsset();

	if (!CurrentDeathAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid death animation to play"));
		return;
	}

	bIsDead = true; // <-- THIS drives the state machine

	UE_LOG(LogTemp, Warning, TEXT("Death triggered. Asset: %s"), *CurrentDeathAsset->GetName());

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
		UE_LOG(LogTemp, Warning, TEXT("No Death Assets available"));
		return;
	}

	int32 Index = FMath::RandRange(0, DeathAssets.Num() - 1);
	CurrentDeathAsset = DeathAssets[Index];

	UE_LOG(LogTemp, Warning, TEXT("Selected Death Asset: %s"),
		*CurrentDeathAsset->GetName());
}

void UCharacterAnimation::ReloadAnimation()
{
	UE_LOG(LogTemp, Error, TEXT("=== ReloadAnimation CALLED | Pawn=%s | Time=%.3f ==="),
		*GetNameSafe(TryGetPawnOwner()),
		GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f);

	if (!ReloadAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("ReloadAsset is null — cannot play reload animation"));
		return;
	}

	PlaySlotAnimationAsDynamicMontage(ReloadAsset, ActionSlotName, 0.25f, 0.25f, 1.0f, 1);
	UE_LOG(LogTemp, Warning, TEXT("Reload animation played: %s"), *ReloadAsset->GetName());
}

void UCharacterAnimation::DeathEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("? Death animation finished — DeathEnded() called."));
	OnDeathEnded.Broadcast();
}

void UCharacterAnimation::AxeAnimation()
{
	if (!AxeAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("AxeAsset is null — cannot play axe animation"));
		return;
	}


	PlaySlotAnimationAsDynamicMontage(AxeAsset, ActionSlotName, 0.25f, 0.25f, 1.0f, 1);
	UE_LOG(LogTemp, Warning, TEXT("Axe animation played: %s"), *AxeAsset->GetName());
}

void UCharacterAnimation::PickaxeAnimation()
{
	if (!PickaxeAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("PickaxeAsset is null"));
		return;
	}

	PlaySlotAnimationAsDynamicMontage(PickaxeAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
}

void UCharacterAnimation::FlashlightAnimation()
{
	if (!FlashlightAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("FlashlightAsset is null"));
		return;
	}

	PlaySlotAnimationAsDynamicMontage(
		FlashlightAsset,
		ActionSlotName,   // must match slot node name in AnimBP
		0.1f, 0.1f, 1.0f, 1);

	UE_LOG(LogTemp, Warning, TEXT("Flashlight animation played: %s"),
		*FlashlightAsset->GetName());
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

	UE_LOG(LogTemp, Warning, TEXT("Footstep notify fired | Pawn=%s | Speed2D=%.2f | Falling=%d"),
		*GetNameSafe(Pawn), Speed2D, bFalling);

	// Gate
	if (!MoveComp->IsMovingOnGround()) return;
	if (Speed2D <= MinFootstepSpeed) return;

	if (!FootstepSound) return;

	UGameplayStatics::PlaySoundAtLocation(Character, FootstepSound, Character->GetActorLocation());
}

void UCharacterAnimation::ZombieAttackAnimation()
{
	if (!ZombieAttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("ZombieAttackAsset is null"));
		return;
	}

	PlaySlotAnimationAsDynamicMontage(ZombieAttackAsset, ActionSlotName, 0.1f, 0.1f, 1.0f, 1);
	UE_LOG(LogTemp, Warning, TEXT("Zombie attack animation played: %s"), *ZombieAttackAsset->GetName());
}