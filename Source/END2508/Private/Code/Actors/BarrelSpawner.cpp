// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/BarrelSpawner.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Code/Actors/MyGameMode.h"

ABarrelSpawner::ABarrelSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	SetRootComponent(BarrelMesh);

	Tags.Add(FName("Enemy"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/END_Starter/Props/RustyBarrel/SM_Barrel_03_Closed.SM_Barrel_03_Closed"));
	if (MeshAsset.Succeeded())
	{
		BarrelMesh->SetStaticMesh(MeshAsset.Object);
	}

	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BarrelMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BarrelMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BarrelMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); 
	BarrelMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);       
	BarrelMesh->SetGenerateOverlapEvents(false);

	CurrentHealth = MaxHealth;
}

void ABarrelSpawner::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	if (HasAuthority() && AgentToSpawn)
	{
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ABarrelSpawner::SpawnAgent, SpawnDelay, true);
	}
}

void ABarrelSpawner::SpawnAgent()
{
	if (bIsDestroyed || !AgentToSpawn) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLoc = GetActorLocation() + FVector(200.f, 0.f, 50.f);
	FRotator SpawnRot = GetActorRotation();

	APawn* NewAgent = GetWorld()->SpawnActor<APawn>(AgentToSpawn, SpawnLoc, SpawnRot, Params);

	if (NewAgent)
	{
		NewAgent->SpawnDefaultController();

		if (AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->RegisterSpawnedEnemy(NewAgent);
		}

		UE_LOG(LogTemp, Warning, TEXT("BarrelSpawner spawned: %s"), *NewAgent->GetName());
	}
}

float ABarrelSpawner::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDestroyed || DamageAmount <= 0.f) return 0.f;

	CurrentHealth -= DamageAmount;

	if (CurrentHealth <= 0.f)
	{
		HandleDestroyed();
	}

	return DamageAmount;
}

void ABarrelSpawner::HandleDestroyed()
{
	bIsDestroyed = true;

	BarrelMesh->SetVisibility(false);
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(SpawnTimer);


	Destroy();
}
