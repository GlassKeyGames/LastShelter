// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamagePickUp.h"
#include "HealthPickUp.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API AHealthPickUp : public ADamagePickUp
{
	GENERATED_BODY()
public:
	AHealthPickUp();

protected:
	virtual void HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult) override;
	virtual void PostPickUp_Implementation() override;

};
