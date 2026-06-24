
#include "Code/Actors/BaseCharacter.h"
#include "Components/ChildActorComponent.h"
#include "Code/Actors/Rifle.h"
#include "Both/CharacterAnimation.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Code/Utility/HealthComponent.h"
#include "TimerManager.h"
#include "KismetAnimationLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Code/Utility/CodeInterface.h"
#include "BuildComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator::ZeroRotator);

	WeaponAttachSocket = FName("WeaponSocket");
	WeaponChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponChild"));
	WeaponChild->SetupAttachment(GetMesh(), WeaponAttachSocket);
	
	if (!WeaponClass)
	{
		static ConstructorHelpers::FClassFinder<ARifle> RifleBPClass(
			TEXT("/Game/Code/Actors/Weapons/BP_CodeRifle"));
		if (RifleBPClass.Succeeded())
		{
			WeaponClass = RifleBPClass.Class;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Weapon] Could not find BP_Rifle class path"));
			WeaponClass = ARifle::StaticClass(); // fallback only
		}
	}

	WeaponChild->SetHiddenInGame(true);
	WeaponChild->SetVisibility(false, true);

	//BuildComponent = CreateDefaultSubobject<UBuildComponent>(TEXT("Build Component"));
}

// BeginPlay
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
		StimuliSource->RegisterWithPerceptionSystem();
	}

	// Weapon init
	if (!WeaponChild)
	{
		UE_LOG(LogTemp, Error, TEXT("[Weapon] WeaponChild is null on %s"), *GetName());
	}
	else
	{
		if (!WeaponClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[Weapon] WeaponClass is null on %s"), *GetName());
		}
		else
		{
			WeaponChild->SetChildActorClass(WeaponClass);

			AActor* Child = WeaponChild->GetChildActor();
			if (!Child)
			{
				WeaponChild->CreateChildActor();
				Child = WeaponChild->GetChildActor();
			}

			if (!Child)
			{
				UE_LOG(LogTemp, Error, TEXT("[Weapon] Child actor failed to create on %s"), *GetName());
			}
			else
			{
				Weapon = Cast<ARifle>(Child);
				if (!Weapon)
				{
					UE_LOG(LogTemp, Error, TEXT("[Weapon] Child actor is not ARifle: %s"), *Child->GetClass()->GetName());
				}
				else
				{
					Weapon->SetOwner(this);
					Weapon->InitializeRifle(this);

					// Keep rifle hidden until equipped in BasePlayer::EquipHotbarSlot
					Weapon->SetActorHiddenInGame(true);
					WeaponChild->SetHiddenInGame(true);
					WeaponChild->SetVisibility(false, true);

					UE_LOG(LogTemp, Warning, TEXT("[Weapon] Initialized rifle: %s"), *Weapon->GetName());
				}
			}
		}
	}

	if (AActor* Child = WeaponChild->GetChildActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] Child class: %s"), *Child->GetClass()->GetName());

		if (ARifle* R = Cast<ARifle>(Child))
		{
			if (USkeletalMeshComponent* M = R->FindComponentByClass<USkeletalMeshComponent>())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Weapon] Rifle mesh asset: %s"),
					M->GetSkeletalMeshAsset() ? *M->GetSkeletalMeshAsset()->GetName() : TEXT("NULL"));
			}
		}
	}

	// Setup animation references
	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		if (FireAnim)
		{
			Anim->SetFireAsset(FireAnim);
		}

		if (HitAnim)
		{
			Anim->HitAsset = HitAnim;
			UE_LOG(LogTemp, Warning, TEXT("Hit animation asset assigned: %s"), *HitAnim->GetName());
		}

		if (DeathAnim1)
		{
			Anim->DeathAssets.Add(DeathAnim1);
			UE_LOG(LogTemp, Warning, TEXT("DeathAnim1 assigned: %s"), *DeathAnim1->GetName());
		}

		if (DeathAnim2)
		{
			Anim->DeathAssets.Add(DeathAnim2);
			UE_LOG(LogTemp, Warning, TEXT("DeathAnim2 assigned: %s"), *DeathAnim2->GetName());
		}
	}

}

void ABaseCharacter::DestroyActorComponent(UActorComponent* ComponentToDestroy)
{
	ComponentToDestroy->DestroyComponent();
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::OnAttack);
	

}

void ABaseCharacter::BuildMode()
{
	if (BuildComponent)
	{
		BuildComponent->ToggleBuildMode();
	}
}



void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttachWeapons();
}

void ABaseCharacter::AttachWeapons()
{
	if (!GetMesh()) return;

	// Attach rifle child actor component to rifle socket
	if (WeaponChild)
	{
		WeaponChild->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponAttachSocket  // BP can override this safely now
		);
	}
}

void ABaseCharacter::OnAttack()
{
	if (bIsDead || !Weapon) return;

	Weapon->Attack(); // Let the weapon handle it all
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* Instig, AActor* Causer)
{
	if (bIsDead || !HealthComponent)
		return 0.f;

	// Always allow damage AND healing (negative damage)
	HealthComponent->TakeDamage(DamageAmount);

	// Play hit animation only if REAL damage occurred (DamageAmount > 0)
	if (DamageAmount > 0.f)
	{
		if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
		{
			Anim->HitAnimation();
		}
	}

	// Death check
	if (HealthComponent->CurrentHealth <= 0.f)
	{
		OnPlayerDeath();
	}

	return DamageAmount;
}

void ABaseCharacter::OnPlayerDeath()
{
	if (bIsDead) return;
	bIsDead = true;

	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(true);
		Weapon->SetIsAlive(false);
	}

	if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		Anim->PlayDeathAnimation();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
}

void ABaseCharacter::EnemyAttack()
{
	if (bIsDead || !Weapon) return;

	bool bFired = Weapon->Attack();
	if (bFired)
	{
		if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
		{
			Anim->FireAnimation();
		}
	}
	else
	{
		// Optional: if out of ammo, stop AI firing behavior here
		UE_LOG(LogTemp, Warning, TEXT("%s cannot fire (enemy)."), *GetName());
	}
}

void ABaseCharacter::HandleActionFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("? BaseCharacter HandleActionFinished executed."));
}

void ABaseCharacter::UpdateHealthHUD(float Current, float Max)
{
	// Default empty implementation, override in ABasePlayer
}

void ABaseCharacter::EnemyReload()
{
	// Default empty implementation — only AI (AAgent) uses it
}

bool ABaseCharacter::CanPickupHealth()
{
	if (!HealthComponent)
		return false;

	return HealthComponent->GetCurrent() < HealthComponent->GetMax();
}

void ABaseCharacter::OnDeathEnded()
{
	Destroy(); // Clean and simple: remove the actor when the death animation ends
}

void ABaseCharacter::HandleDeath(float Ratio)
{
	bIsDead = true;

	// Optional: stop firing, hide mesh, etc.
	UE_LOG(LogTemp, Warning, TEXT("BaseCharacter HandleDeath called. Ratio: %.2f"), Ratio);
}