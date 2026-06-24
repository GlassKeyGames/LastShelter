// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/AmmoPickUp.h"
#include "Components/StaticMeshComponent.h"
#include "Code/Actors/BasePlayer.h" 
#include "Kismet/GameplayStatics.h"

AAmmoPickUp::AAmmoPickUp()
{
    UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CrateMesh(TEXT("/Game/END_Starter/Props/MilitaryCrates/SM_MilitaryCrateClosed_03.SM_MilitaryCrateClosed_03"));
    if (CrateMesh.Succeeded())
    {
        Mesh->SetStaticMesh(CrateMesh.Object);
    }

    if (BoxCollision)
    {
        BoxCollision->SetupAttachment(Mesh);
    }
}

void AAmmoPickUp::HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn)
        return;

    // Reject AI completely
    if (!Pawn->IsPlayerControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("Ammo pickup: ignored AI pawn"), *Pawn->GetName());
        return; 
    }

    if (ABasePlayer* Player = Cast<ABasePlayer>(Pawn))
    {
        Player->AddMaxAmmo(PickupData.AmmoAmount);
        Player->HandleAmmoQuest();

        UE_LOG(LogTemp, Warning, TEXT("Ammo pickup: picked up by PLAYER"));

        Destroy();
    }
}

void AAmmoPickUp::PostPickUp_Implementation()
{
    
}