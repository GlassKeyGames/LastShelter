// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Actors/BasePickUp.h"
#include "DamagePickUp.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API ADamagePickUp : public ABasePickUp
{
	GENERATED_BODY()
public:
	ADamagePickUp();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 2.0f;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSystem;

	virtual void HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult) override;
};
