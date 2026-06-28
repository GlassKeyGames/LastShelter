// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Flashlight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"

AFlashlight::AFlashlight()
{
    PrimaryActorTick.bCanEverTick = false;

    FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
    SetRootComponent(FlashlightMesh);
    FlashlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(FlashlightMesh);

    // Good default values (tweak in BP)
    SpotLight->Intensity = 15000.f;
    SpotLight->AttenuationRadius = 1800.f;
    SpotLight->InnerConeAngle = 12.f;
    SpotLight->OuterConeAngle = 28.f;

    bIsOn = false;
}

void AFlashlight::BeginPlay()
{
    Super::BeginPlay();
    SetLightEnabled(bStartOn);
}

void AFlashlight::ToggleLight()
{

    SetLightEnabled(!bIsOn);

    // play sound
    if (bIsOn && ToggleOnSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ToggleOnSound, GetActorLocation());
    }
    else if (!bIsOn && ToggleOffSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ToggleOffSound, GetActorLocation());
    }
}

void AFlashlight::SetLightEnabled(bool bEnabled)
{
    bIsOn = bEnabled;

    if (SpotLight)
    {
        SpotLight->SetVisibility(bIsOn, true);
        SpotLight->SetHiddenInGame(!bIsOn, true);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Flashlight] %s"), bIsOn ? TEXT("ON") : TEXT("OFF"));
}
