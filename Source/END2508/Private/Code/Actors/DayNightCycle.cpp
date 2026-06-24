// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/DayNightCycle.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UnrealType.h"

// Sets default values
ADayNightCycle::ADayNightCycle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();



}

// Called every frame
void ADayNightCycle::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bRunCycle || !SunLight || DayLengthSeconds <= 0.f)
    {
        return;
    }

    // -----------------------------
    // 1) Advance normalized time [0..1)
    // -----------------------------
    TimeOfDay += DeltaSeconds / DayLengthSeconds;
    TimeOfDay = FMath::Fmod(TimeOfDay, 1.0f);
    if (TimeOfDay < 0.f)
    {
        TimeOfDay += 1.0f;
    }

    // -----------------------------
    // 2) Rotate Sun & Moon
    // -----------------------------
    const float SunPitch = TimeOfDay * 360.0f - 90.0f;
    SunLight->SetActorRotation(FRotator(SunPitch, -90.0f, 0.0f));

    if (MoonLight)
    {
        const float MoonPitch = SunPitch + 180.0f;
        MoonLight->SetActorRotation(FRotator(MoonPitch, -90.0f, 0.0f));
    }

    // -----------------------------
    // 3) Day/Night blend values
    // -----------------------------
    // Smooth day factor (0 = night, 1 = full day, smooth around sunrise/sunset)
    const float DayAlpha = FMath::Clamp(FMath::Sin(TimeOfDay * 2.0f * PI), 0.0f, 1.0f);
    const float NightAlpha = 1.0f - DayAlpha;

    // Deep night = only high in the middle of the night (keeps sunsets orange)
    const float DeepNightAlpha = FMath::Clamp((NightAlpha - 0.6f) / 0.3f, 0.0f, 1.0f);
    const bool bIsDeepNightNow = DeepNightAlpha > 0.0f;

    // -----------------------------
    // 4) Light intensities
    // -----------------------------
    if (SunLight && SunLight->GetLightComponent())
    {
        const float SunIntensity = 10.0f * DayAlpha;  // strong in day, fades out at night
        SunLight->GetLightComponent()->SetIntensity(SunIntensity);
    }

    if (MoonLight && MoonLight->GetLightComponent())
    {
        // Moon only really visible during deep night
        const float MoonIntensity = 0.3f * DeepNightAlpha;
        MoonLight->GetLightComponent()->SetIntensity(MoonIntensity);
    }

    // -----------------------------
    // 5) Drive BP_Sky_Sphere (stars + active light)
    // -----------------------------
    if (SkySphereActor)
    {
        // Use sun for day / sunset, moon for deep night
        ADirectionalLight* ActiveSkyLight =
            (bIsDeepNightNow && MoonLight) ? MoonLight : SunLight;

        static const FName DirLightVarName(TEXT("Directional light actor"));
        static const FName StarsVarName(TEXT("Stars brightness"));
        static const FName RefreshFnName(TEXT("RefreshMaterial"));

        // Set "Directional light actor"
        if (FObjectProperty* ObjProp = CastField<FObjectProperty>(
            SkySphereActor->GetClass()->FindPropertyByName(DirLightVarName)))
        {
            ObjProp->SetObjectPropertyValue_InContainer(SkySphereActor, ActiveSkyLight);
        }

        // Stars: full brightness only during deep night
        if (FFloatProperty* FloatProp = CastField<FFloatProperty>(
            SkySphereActor->GetClass()->FindPropertyByName(StarsVarName)))
        {
            const float StarBrightness = DeepNightAlpha * MaxStarsBrightness;
            FloatProp->SetPropertyValue_InContainer(SkySphereActor, StarBrightness);
        }

        // Apply on BP_Sky_Sphere
        if (UFunction* RefreshFn = SkySphereActor->FindFunction(RefreshFnName))
        {
            SkySphereActor->ProcessEvent(RefreshFn, nullptr);
        }
    }

    // -----------------------------
    // 6) Optional: hide volumetric clouds at night
    //     (Add AActor* VolumetricCloudActor to your header and assign in level)
    // -----------------------------
    if (VolumetricCloudActor)
    {
        const bool bShowClouds = (DeepNightAlpha <= 0.0f);  // show only when NOT deep night
        VolumetricCloudActor->SetActorHiddenInGame(!bShowClouds);
    }

    // -----------------------------
    // 7) Skylight recapture only on day/night state change
    // -----------------------------
    if (SkyLightActor && SkyLightActor->GetLightComponent())
    {
        static bool bWasDeepNight = false;

        if (bIsDeepNightNow != bWasDeepNight)
        {
            // We just crossed into or out of deep night -> recapture once
            SkyLightActor->GetLightComponent()->RecaptureSky();
            bWasDeepNight = bIsDeepNightNow;
        }
    }
}