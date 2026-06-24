// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rifle.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRifleAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRifleActionStopped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, float, Current, float, Max);

UCLASS()
class END2508_API ARifle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARifle();

protected:
	virtual void BeginPlay() override;

	// Mesh for the rifle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* RifleMesh;

	// Projectile to fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	// Animation for firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimSequence* FireAsset;

	// How fast the rifle fires (time between shots)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate = 1.0f;

	// Firing cooldown
	FTimerHandle FireTimer;

	// Handles ammo deduction logic
	bool UseAmmo();

public:
	// Initialize with owning pawn
	UFUNCTION()
	void InitializeRifle(APawn* InOwnerPawn);

	// Start a shot
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool Attack();

	// Reset firing cooldown
	UFUNCTION()
	void ResetAction();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadAmmo();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CompleteReload();

	// Getters
	float GetCurrentAmmo() const { return CurrentAmmo; }
	float GetMaxAmmo() const { return MaxAmmo; }

	// Used by AI/Player logic
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bActionHappening = false;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsActionHappening() const;

	UFUNCTION(BlueprintCallable)
	void SetIsAlive(bool bInAlive);

	// Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float MaxAmmo = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	float CurrentAmmo = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool bUsesAmmo = true;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bIsAlive = true;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	APawn* ParentPawn;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRifleAttack OnRifleAttack;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnAmmoChanged;

	bool bIsAIWeapon = false;

	void SetIsAIWeapon(bool bAI) { bIsAIWeapon = bAI; }

	UFUNCTION(BlueprintCallable)
	void SetMaxAmmo(float NewMax);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* FireSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ReloadSound = nullptr;
};