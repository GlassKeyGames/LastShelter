// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Torch.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"


ATorch::ATorch()
{
    PrimaryActorTick.bCanEverTick = false;

    TorchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
    SetRootComponent(TorchMesh);

    FireFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFX"));
    FireFX->SetupAttachment(TorchMesh);
    FireFX->bAutoActivate = false;

    TorchSound = CreateDefaultSubobject<UAudioComponent>(TEXT("TorchSound"));
    TorchSound->SetupAttachment(TorchMesh);
    TorchSound->bAutoActivate = false;

    TorchLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TorchLight"));
    TorchLight->SetupAttachment(TorchMesh);
    TorchLight->SetMobility(EComponentMobility::Movable);
    TorchLight->Intensity = 5000.f;
    TorchLight->AttenuationRadius = 800.f;
    TorchLight->bUseInverseSquaredFalloff = false;
    TorchLight->SetVisibility(false);
}

void ATorch::BeginPlay()
{
    Super::BeginPlay();

    if (FireFX)
    {
        FireFX->Activate(true);
    }

    if (TorchSound)
    {
        TorchSound->Play();
    }

    if (TorchLight)
    {
        TorchLight->SetVisibility(true);
    }
}