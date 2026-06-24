// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Actors/BasePickUp.h"
#include "AmmoPickUp.generated.h"

USTRUCT(BlueprintType)
struct FAmmoPickupData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoAmount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool bIsValid = true;
};

UCLASS()
class END2508_API AAmmoPickUp : public ABasePickUp
{
	GENERATED_BODY()
public:
	AAmmoPickUp();
	virtual void PostPickUp_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FAmmoPickupData PickupData;

	virtual void HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult) override;
};
