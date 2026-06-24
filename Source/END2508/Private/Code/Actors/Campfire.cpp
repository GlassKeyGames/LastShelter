// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Campfire.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"

ACampfire::ACampfire()
{
    PrimaryActorTick.bCanEverTick = false;

    CampfireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CampfireMesh"));
    SetRootComponent(CampfireMesh);

    FireFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFX"));
    FireFX->SetupAttachment(CampfireMesh);
    FireFX->bAutoActivate = false;

    FireSound = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSound"));
    FireSound->SetupAttachment(CampfireMesh);
    FireSound->bAutoActivate = false;

    FireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireLight"));
    FireLight->SetupAttachment(CampfireMesh);
    FireLight->SetMobility(EComponentMobility::Movable);
    FireLight->Intensity = 400.f;          // softer than before
    FireLight->AttenuationRadius = 600.f;
    FireLight->bUseInverseSquaredFalloff = false;
    FireLight->SetVisibility(false);
}

void ACampfire::BeginPlay()
{
    Super::BeginPlay();

    if (FireFX)
    {
        FireFX->Activate(true);
    }

    if (FireSound)
    {
        FireSound->Play();
    }

    if (FireLight)
    {
        FireLight->SetVisibility(true);
    }
}
