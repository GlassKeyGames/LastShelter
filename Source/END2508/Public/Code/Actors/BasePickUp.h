// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"  
#include "BasePickUp.generated.h"

UCLASS(Abstract)
class END2508_API ABasePickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABasePickUp();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UBoxComponent* BoxCollision;

	UFUNCTION()
	void BoundOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void HandlePickUp(AActor* OtherActor, const FHitResult& SweepResult);
	virtual void HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void PostPickUp();
	virtual void PostPickUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Pickup")
	bool CanPickup() const;
};