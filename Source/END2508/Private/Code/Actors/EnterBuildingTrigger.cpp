// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/EnterBuildingTrigger.h"
#include "Components/BoxComponent.h"
#include "Code/Actors/BasePlayer.h"

// Sets default values
AEnterBuildingTrigger::AEnterBuildingTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    SetRootComponent(BoxCollision);

    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxCollision->SetGenerateOverlapEvents(true);
    BoxCollision->SetBoxExtent(FVector(300.f, 300.f, 300.f));
}

// Called when the game starts or when spawned
void AEnterBuildingTrigger::BeginPlay()
{
    Super::BeginPlay();

    if (BoxCollision)
    {
        BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnterBuildingTrigger::OnBeginOverlap);
        BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AEnterBuildingTrigger::OnEndOverlap);
    }
}

void AEnterBuildingTrigger::OnBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (ABasePlayer* Player = Cast<ABasePlayer>(OtherActor))
    {
        Player->ForceFirstPerson(true);
    }
}

void AEnterBuildingTrigger::OnEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (ABasePlayer* Player = Cast<ABasePlayer>(OtherActor))
    {
        Player->ForceFirstPerson(false);
    }
}
