// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Utility/HealthComponent.h"
#include "RegenHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API URegenHealthComponent : public UHealthComponent
{
	GENERATED_BODY()
	
	URegenHealthComponent();

public:
	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TurnTickOff();

	UFUNCTION()
	void TurnTickOn();

	FTimerHandle RegenTimerHandle;

	virtual void HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	virtual void HandleDeath(float Ratio);
};
