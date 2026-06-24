// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Actors/BaseCharacter.h"
#include "Code/Utility/CodeInterface.h"
#include "Agent.generated.h"

class ARifle;
class UAudioComponent;
class USoundBase;
DECLARE_MULTICAST_DELEGATE(FOnActionStopped)

UCLASS()
class END2508_API AAgent : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AAgent();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void EnemyAttack() override;
	virtual void HandleActionFinished() override;
	virtual void EnemyReload() override;
	virtual void HandleDeath(float Ratio) override;

	UFUNCTION()
	void HandleHurt(float CurrentHealth, float MaxHealth);
	
	void UpdateBlackboardHealth(float CurrentHealth, float MaxHealth);

	FOnActionStopped OnActionStopped;

	FTimerHandle FireLoopHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ARifle> RifleClass;

	UFUNCTION()
	void UpdateBlackboardAmmo(float Current, float Max);

	UFUNCTION()
	void DestroySelfAfterDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool bIsZombie = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeRange = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeDamage = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackInterval = 1.2f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttackOnCooldown = false;

	FTimerHandle AttackCooldownHandle;

	UFUNCTION()
	void ResetAttackCooldown();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsPlayerInMeleeRange() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ZombieLoopSound = nullptr;

	UPROPERTY()
	UAudioComponent* ZombieAudioComponent = nullptr;
};
