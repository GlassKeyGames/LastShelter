// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Torch.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class UAudioComponent;
class UPointLightComponent;

UCLASS()
class END2508_API ATorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorch();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* TorchMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UParticleSystemComponent* FireFX;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAudioComponent* TorchSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPointLightComponent* TorchLight;

};
