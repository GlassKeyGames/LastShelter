// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/PickUps.h"
#include "Code/Actors/BasePlayer.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Both/PlayerHUD.h"

// Sets default values
APickUps::APickUps()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupSkeletalMesh"));
	PickupSkeletalMesh->SetupAttachment(Root);
	PickupSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Default: hide skeletal unless you assign one in BP
	PickupSkeletalMesh->SetHiddenInGame(true);


	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);
	PickupSphere->SetSphereRadius(120.f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void APickUps::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUps::OnOverlapBegin);
	PickupSphere->OnComponentEndOverlap.AddDynamic(this, &APickUps::OnOverlapEnd);

	// Fallback so slot data always has a class
	if (!PickupItemClass)
	{
		PickupItemClass = GetClass();
	}

	// ? Auto show whichever mesh is set (no extra enum needed)
	const bool bHasSkeletal = (PickupSkeletalMesh && PickupSkeletalMesh->GetSkeletalMeshAsset() != nullptr);
	const bool bHasStatic = (PickupMesh && PickupMesh->GetStaticMesh() != nullptr);

	if (bHasSkeletal)
	{
		PickupSkeletalMesh->SetHiddenInGame(false);
		PickupMesh->SetHiddenInGame(true);
	}
	else if (bHasStatic)
	{
		PickupMesh->SetHiddenInGame(false);
		PickupSkeletalMesh->SetHiddenInGame(true);
	}
	else
	{
		// nothing assigned
		PickupMesh->SetHiddenInGame(true);
		PickupSkeletalMesh->SetHiddenInGame(true);
	}
}

void APickUps::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
	if (!Player) return;

	Player->SetCurrentPickup(this);

	if (UPlayerHUD* HUD = Player->GetHUDWidget())
	{
		HUD->SetInteractTextVisible(true, PickupText);
	}
}

void APickUps::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
	if (!Player) return;

	// Only clear if we are the current target
	if (Player->GetCurrentPickup() == this)
	{
		Player->SetCurrentPickup(nullptr);

		if (UPlayerHUD* HUD = Player->GetHUDWidget())
		{
			HUD->SetInteractTextVisible(false, FText::GetEmpty());
		}
	}
}

bool APickUps::TryPickup(ABasePlayer* Player)
{
	if (!Player) return false;
	if (PickupType == EEquipState::Unarmed) return false;

	int32 NewIndex = INDEX_NONE;
	const bool bAdded = Player->AddToFirstAvailableSlot(PickupItemClass, PickupIcon, PickupType, NewIndex);
	if (!bAdded) return false;
	Player->HandlePickupQuest(PickupType);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		
	}

	Player->EquipHotbarSlot(NewIndex);

	if (UPlayerHUD* HUD = Player->GetHUDWidget())
	{
		HUD->SetInteractTextVisible(false, FText::GetEmpty());
	}

	// ? kill the visible weapon actor in the level
	if (WorldWeaponActor)
	{
		WorldWeaponActor->Destroy();
		WorldWeaponActor = nullptr;
	}

	Destroy();
	return true;
}
