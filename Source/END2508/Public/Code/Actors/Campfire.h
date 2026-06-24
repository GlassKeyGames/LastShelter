// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Campfire.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class UAudioComponent;
class UPointLightComponent;

UCLASS()
class END2508_API ACampfire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACampfire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* CampfireMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UParticleSystemComponent* FireFX;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAudioComponent* FireSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPointLightComponent* FireLight;

};
