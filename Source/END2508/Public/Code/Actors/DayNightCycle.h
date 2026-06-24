// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycle.generated.h"

class ADirectionalLight;
class ASkyLight;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class AActor;

UCLASS()
class END2508_API ADayNightCycle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADayNightCycle();
    virtual void Tick(float DeltaSeconds) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

    // Directional light in scene (your sun)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    ADirectionalLight* SunLight = nullptr;

    // Optional skylight for recapture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    ASkyLight* SkyLightActor = nullptr;

    // How long one full day takes in real seconds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight", meta = (ClampMin = "10.0"))
    float DayLengthSeconds = 60.0f;

    // 0..1 (0 = midnight, 0.25 = sunrise, 0.5 = noon, 0.75 = sunset)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TimeOfDay = 0.25f; // start near sunrise

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    bool bRunCycle = true;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    AActor* SkySphereActor = nullptr;

    // Max stars brightness at full night
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Sky", meta = (ClampMin = "0.0"))
    float MaxStarsBrightness = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    ADirectionalLight* MoonLight = nullptr;

    UPROPERTY(EditAnywhere, Category = "Sky")
    AActor* VolumetricCloudActor = nullptr;


private:
	float SkyRecaptureTimer = 0.f;

 

};
