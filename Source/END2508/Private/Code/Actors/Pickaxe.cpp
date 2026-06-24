// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Pickaxe.h"
#include "Code/Actors/HittingResources.h"
#include "Code/Actors/BasePlayer.h"
#include "DrawDebugHelpers.h"

APickaxe::APickaxe()
{
    // Optional tuning
    //Damage = 35.f;   // different from axe if desired
    Range = 100.f;
    Radius = 75.f;
}

bool APickaxe::Attack()
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor) return false;

	const FVector Start = OwningActor->GetActorLocation() + FVector(0.f, 0.f, 30.f);
	const FVector End = Start + OwningActor->GetActorForwardVector() * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PickaxeAttack), false);
	Params.AddIgnoredActor(OwningActor);
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

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 1.0f, 0, 1.5f);
	DrawDebugSphere(GetWorld(), bHit ? Hit.ImpactPoint : End, Radius, 12, bHit ? FColor::Blue : FColor::Cyan, false, 1.0f);
#endif

	if (!bHit || !Hit.GetActor())
	{
		return false;
	}

	// Pickaxe = Stone only
	if (AHittingResources* Node = Cast<AHittingResources>(Hit.GetActor()))
	{
		if (Node->GetResourceType() == EResourceType::Stone)
		{
			if (ABasePlayer* Player = Cast<ABasePlayer>(OwningActor))
			{
				Node->HandleResourceHit(Player);
			}
			return true;
		}
	}

	return false;
}