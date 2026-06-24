// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/HittingResources.h"
#include "Components/StaticMeshComponent.h"
#include "Code/Actors/BasePlayer.h"

AHittingResources::AHittingResources()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(ECC_WorldStatic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AHittingResources::BeginPlay()
{
    Super::BeginPlay();
}

void AHittingResources::HandleResourceHit(AActor* PlayerActor)
{
    ABasePlayer* Player = Cast<ABasePlayer>(PlayerActor);
    if (!Player) return;

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastHitTime < HitCooldown) return;
    LastHitTime = Now;

    if (ResourceType == EResourceType::Wood)
    {
        Player->AddWood(AmountPerHit);
    }
    else // Stone
    {
        Player->AddStone(AmountPerHit);
    }
}