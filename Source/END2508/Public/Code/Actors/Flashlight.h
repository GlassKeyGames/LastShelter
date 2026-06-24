// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flashlight.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;

UCLASS()
class END2508_API AFlashlight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlashlight();

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void ToggleLight();

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void SetLightEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Flashlight")
    bool IsLightOn() const { return bIsOn; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* ToggleOnSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* ToggleOffSound = nullptr;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* FlashlightMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpotLightComponent* SpotLight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    bool bStartOn = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
    bool bIsOn = false;
};
