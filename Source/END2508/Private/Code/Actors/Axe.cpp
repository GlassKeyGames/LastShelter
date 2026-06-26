// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Axe.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Code/Actors/HittingResources.h"
#include "Code/Actors/BasePlayer.h"

// Sets default values
AAxe::AAxe()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Root);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetSimulatePhysics(false);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Root);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetSimulatePhysics(false);

	// Pick ONE to show by default
	StaticMesh->SetHiddenInGame(false);
	SkeletalMesh->SetHiddenInGame(true);

	Range = 50.f;
	Radius = 45.f;

}

void AAxe::InitializeAxe(AActor* InOwner)
{
	OwnerActor = InOwner;
	SetOwner(InOwner);
}

bool AAxe::Attack()
{
	if (!OwnerActor) return false;

	const FVector Start = OwnerActor->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector End = Start + OwnerActor->GetActorForwardVector() * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AxeAttack), false);
	Params.AddIgnoredActor(OwnerActor);
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(Radius),
		Params
	);


	if (!bHit || !Hit.GetActor())
	{
		return false;
	}

	// Resource hit
	if (AHittingResources* Node = Cast<AHittingResources>(Hit.GetActor()))
	{
		if (Node->GetResourceType() == EResourceType::Wood) // axe = wood
		{
			if (ABasePlayer* Player = Cast<ABasePlayer>(GetOwner()))
			{
				Node->HandleResourceHit(Player);
			}
		}
		return true;
	}

	// Normal damage hit
	UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);
	return true;
}

