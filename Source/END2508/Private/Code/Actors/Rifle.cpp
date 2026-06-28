// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Rifle.h"
#include "Both/CharacterAnimation.h"
#include "UObject/ConstructorHelpers.h"                 
#include "Components/SkeletalMeshComponent.h"           
#include "GameFramework/PlayerController.h"             
#include "GameFramework/Character.h"
#include "Engine/Engine.h"                              
#include "Engine/World.h"
#include "Code/Utility/CodeInterface.h"
#include "TimerManager.h"
#include "AIController.h"                              
#include "Code/Actors/Agent.h"                          
#include "BehaviorTree/BlackboardComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ARifle::ARifle()
{
	PrimaryActorTick.bCanEverTick = false;

	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));
	SetRootComponent(RifleMesh);
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MaxAmmo = 3.f;
	CurrentAmmo = MaxAmmo;
	bIsAlive = true;
	
}

void ARifle::BeginPlay()
{
    Super::BeginPlay();

    MaxAmmo = FMath::Max(0.f, MaxAmmo);
    CurrentAmmo = FMath::Clamp(CurrentAmmo, 0.f, MaxAmmo);

    UE_LOG(LogTemp, Warning, TEXT("? BeginPlay: MaxAmmo = %.0f | CurrentAmmo = %.0f"), MaxAmmo, CurrentAmmo);
}

void ARifle::InitializeRifle(APawn* InOwnerPawn)
{
    ParentPawn = InOwnerPawn;

    if (InOwnerPawn)
    {
        SetOwner(InOwnerPawn);
    }

    if (RifleMesh)
    {
        RifleMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    }

    if (ParentPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Rifle initialized for: %s"), *ParentPawn->GetName());
    }
}

bool ARifle::Attack()
{
    if (bActionHappening)
    {
        UE_LOG(LogTemp, Warning, TEXT("Still firing, can't shoot again yet."));
        return false;
    }

    if (!bIsAlive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dead, can't fire."));
        return false;
    }

    if (!UseAmmo())
    {
        UE_LOG(LogTemp, Warning, TEXT("Out of ammo — can't fire!"));
        return false;
    }

    if (!ProjectileClass || !ParentPawn || !RifleMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT(" Attack failed"));
        return false;
    }

    bActionHappening = true;

    FVector MuzzleLoc = RifleMesh->DoesSocketExist("MuzzleFlashSocket")
        ? RifleMesh->GetSocketLocation("MuzzleFlashSocket")
        : RifleMesh->GetComponentLocation();

    FRotator MuzzleRot = RifleMesh->DoesSocketExist("MuzzleFlashSocket")
        ? RifleMesh->GetSocketRotation("MuzzleFlashSocket")
        : RifleMesh->GetComponentRotation();

    FVector TargetPoint = FVector::ZeroVector;
    AController* Controller = ParentPawn->GetController();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        FVector CamLoc;
        FRotator CamRot;
        PC->GetPlayerViewPoint(CamLoc, CamRot);
        TargetPoint = CamLoc + CamRot.Vector() * 10000.f;

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(ParentPawn);
        Params.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TargetPoint, ECC_Visibility, Params))
        {
            const float DistanceFromMuzzle = FVector::Dist(MuzzleLoc, Hit.ImpactPoint);

            if (DistanceFromMuzzle > 300.f)
            {
                TargetPoint = Hit.ImpactPoint;
            }
        }
    }
    else if (AAIController* AICon = Cast<AAIController>(Controller))
    {
        if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
        {
            if (AActor* Target = Cast<AActor>(BB->GetValueAsObject("Player")))
            {
                TargetPoint = Target->GetActorLocation();
            }
        }
    }

    if (TargetPoint.IsZero())
    {
        TargetPoint = MuzzleLoc + MuzzleRot.Vector() * 10000.f;
    }

    FVector FireDir = (TargetPoint - MuzzleLoc).GetSafeNormal();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        FVector CamLoc;
        FRotator CamRot;
        PC->GetPlayerViewPoint(CamLoc, CamRot);

        const FVector CameraForward = CamRot.Vector();

        // If the aim direction points backward compared to the camera,
        // force the bullet to shoot forward from the camera direction.
        if (FVector::DotProduct(FireDir, CameraForward) < 0.25f)
        {
            FireDir = CameraForward;
        }
    }

    FRotator FireRot = FireDir.Rotation();


    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = ParentPawn;
    SpawnParams.Instigator = ParentPawn;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector SpawnLoc = MuzzleLoc + (FireDir * 150.f);

    GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLoc, FireRot, SpawnParams);

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            FireSound,
            GetActorLocation()
        );
    }

    // Play fire animation
    if (ACharacter* Character = Cast<ACharacter>(ParentPawn))
    {
        if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
        {
            if (UCharacterAnimation* CharAnim = Cast<UCharacterAnimation>(AnimInst))
            {
                CharAnim->FireAnimation();
            }
        }
    }

    OnRifleAttack.Broadcast();

    float Delay = FireAsset ? FireAsset->GetPlayLength() : FireRate;
    GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ARifle::ResetAction, Delay, false);

    return true;
}

void ARifle::ResetAction()
{
	bActionHappening = false;

	if (ParentPawn && ParentPawn->GetClass()->ImplementsInterface(UCodeInterface::StaticClass()))
	{
		if (ICodeInterface* Interface = Cast<ICodeInterface>(ParentPawn))
		{
			Interface->HandleActionFinished();
		}
	}
}

void ARifle::ReloadAmmo()
{
    if (bIsAIWeapon)
    {
        CompleteReload();
        bActionHappening = false;
        return;
    }

    bActionHappening = true; 

    if (ReloadSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ReloadSound,
            GetActorLocation()
        );
    }
}

bool ARifle::UseAmmo()
{
    if (bIsAIWeapon) return true;

    if (CurrentAmmo > 0.f)
    {
        CurrentAmmo--;
        OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
        return true;
    }

    return false;
}

void ARifle::SetIsAlive(bool bInAlive)
{
	bIsAlive = bInAlive;
}

bool ARifle::IsActionHappening() const
{
    return bActionHappening;
}

void ARifle::CompleteReload()
{
    CurrentAmmo = MaxAmmo;
    if (ParentPawn && ParentPawn->IsPlayerControlled())
    {
        OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
    }
}

void ARifle::SetMaxAmmo(float NewMax)
{
    MaxAmmo = FMath::Max(NewMax, 0.f);
    CurrentAmmo = FMath::Min(CurrentAmmo, MaxAmmo); // Optional: keep current ammo in range
    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
}