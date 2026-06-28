// Fill out your copyright notice in the Description page of Project Settings.

#include "Code/Actors/Agent.h"
#include "Code/Actors/Rifle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Code/Actors/MyGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Both/CharacterAnimation.h"
#include "../END2508.h"

AAgent::AAgent()
	: Super(FObjectInitializer::Get())
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCapsuleHalfHeight(130.f);
	GetCapsuleComponent()->SetCapsuleRadius(65.f);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Game/Both/DoNotConvert/ABP_ZombieCharacterAnimation.ABP_ZombieCharacterAnimation_C"));
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}
}

void AAgent::BeginPlay()
{
	Super::BeginPlay();


	if (!bIsZombie && RifleClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ARifle* SpawnedRifle = GetWorld()->SpawnActor<ARifle>(RifleClass, SpawnParams);

		if (SpawnedRifle)
		{
			Weapon = SpawnedRifle;

			FName WeaponSocket = TEXT("WeaponSocket");
			SpawnedRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);

			Weapon->InitializeRifle(this);
			Weapon->SetIsAIWeapon(true);
			Weapon->bUsesAmmo = false;

			Weapon->OnAmmoChanged.AddDynamic(this, &AAgent::UpdateBlackboardAmmo);
		}
	
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn rifle for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RifleClass not set for %s"), *GetName());
	}

	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		
		Anim->OnDeathEnded.AddDynamic(this, &AAgent::DestroySelfAfterDeath);
	}

	if (bIsZombie && ZombieLoopSound)
	{
		ZombieAudioComponent = NewObject<UAudioComponent>(this);
		if (ZombieAudioComponent)
		{
			ZombieAudioComponent->SetupAttachment(GetRootComponent());
			ZombieAudioComponent->RegisterComponent();
			ZombieAudioComponent->SetSound(ZombieLoopSound);
			ZombieAudioComponent->bAutoActivate = false;
		}
	}
}

void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsZombie || !ZombieAudioComponent)
	{
		return;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Player)
	{
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	const float HearDistance = 700.0f; // feet in cm

	if (Distance <= HearDistance)
	{
		if (!ZombieAudioComponent->IsPlaying())
		{
			ZombieAudioComponent->Play();
		}
	}
	else
	{
		if (ZombieAudioComponent->IsPlaying())
		{
			ZombieAudioComponent->Stop();
		}
	}
}

void AAgent::EnemyAttack()
{
	if (bIsDead || bIsAttackOnCooldown || bIsAttacking) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Player)
	{
		HandleActionFinished();
		return;
	}

	bIsAttacking = true;
	bIsAttackOnCooldown = true;

	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsZombieAttacking = true;
	}

	UGameplayStatics::ApplyDamage(Player, MeleeDamage, GetController(), this, nullptr);

	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		this,
		&AAgent::ResetAttackCooldown,
		AttackInterval,
		false
	);

	GetWorldTimerManager().SetTimer(
		FireLoopHandle,
		this,
		&AAgent::HandleActionFinished,
		AttackInterval,
		false
	);
}

void AAgent::HandleActionFinished()
{
	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsZombieAttacking = false;
	}

	if (OnActionStopped.IsBound())
	{
		OnActionStopped.Broadcast();
	}
}

void AAgent::HandleHurt(float CurrentHealth, float MaxHealth)
{
	UpdateBlackboardHealth(CurrentHealth, MaxHealth);

	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsTakingDamage = true;
	}
}

void AAgent::UpdateBlackboardHealth(float CurrentHealth, float MaxHealth)
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			const float Ratio = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
			BB->SetValueAsFloat("Health Ratio", Ratio);
		}
	}
}

void AAgent::EnemyReload()
{
	if (Weapon)
	{
		// Start reload (AI reloads instantly)
		Weapon->ReloadAmmo();

		if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
		{
			Anim->ReloadAnimation();
		}

		// DELAY the finish message so AI stays behind cover
		GetWorldTimerManager().SetTimer(
			FireLoopHandle,
			this,
			&AAgent::HandleActionFinished,
			2.0f,    // TIME AI STAYS IN COVER
			false
		);
	}
}

void AAgent::UpdateBlackboardAmmo(float Current, float Max)
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			// Update just the current ammo value
			BB->SetValueAsFloat("Ammo", Current); 
		}
	}
}

void AAgent::HandleDeath(float Ratio)
{
	Super::HandleDeath(Ratio); // Call parent/base logic

	// Detach from AI Controller
	DetachFromControllerPendingDestroy();

	UE_LOG(LogTemp, Warning, TEXT("Agent detached from controller after death: %s"), *GetName());
}

void AAgent::DestroySelfAfterDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Agent DestroySelfAfterDeath called: %s"), *GetName());

	Destroy();
}

void AAgent::ResetAttackCooldown()
{
	bIsAttackOnCooldown = false;
	bIsAttacking = false;
}

bool AAgent::IsPlayerInMeleeRange() const
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Player) return false;

	const float Dist2D = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation());
	return Dist2D <= MeleeRange;
}