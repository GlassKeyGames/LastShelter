// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Code/EquipedState.h"
#include "CharacterAnimation.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadNow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEnded);

UCLASS()
class END2508_API UCharacterAnimation : public UAnimInstance
{
	GENERATED_BODY()
	

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Default)
    float Velocity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Default)
    float Direction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    FName ActionSlotName = "Action";

public:
    UCharacterAnimation();

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeInitializeAnimation() override;
 
    virtual void PreviewWindowUpdate_Implementation();

    // --- Fire ---
    UFUNCTION(BlueprintCallable)
    void FireAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetFireAsset(UAnimSequence* NewFireAsset);

    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    bool bShouldFire = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire")
    UAnimSequence* FireAsset;

    // --- Hit ---
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void HitAnimation();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimSequence* HitAsset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimSequence* HurtAsset = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
    USoundBase* HurtSound = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
    bool bIsTakingDamage = false;

    // --- Death ---
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayDeathAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SelectDeathAsset();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TArray<UAnimSequence*> DeathAssets;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    UAnimSequence* CurrentDeathAsset;

    UPROPERTY(BlueprintReadOnly, Category = "Death")
    bool bIsDead = false;

    UPROPERTY(BlueprintAssignable, Category = "Death")
    FOnDeathEnded OnDeathEnded;

    // --- Reload ---
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void ReloadAnimation();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reload")
    UAnimSequence* ReloadAsset;

    UPROPERTY(BlueprintAssignable, Category = "Reload")
    FOnReloadNow OnReloadNow;

    UPROPERTY(BlueprintAssignable, Category = "Reload")
    FOnReloadNow OnReloadEnded;

    UFUNCTION()
    void DeathEnded();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool IsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "Character")
    APawn* OwningPawn;

    // Exposed to the AnimGraph for blendspace control
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float JumpAlpha = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Jump")
    bool bJustJumped = false;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    bool bHoldingAxe = false;

    UPROPERTY(BlueprintReadOnly, Category = "Equipment")
    EEquipState EquipState = EEquipState::Unarmed;

    // --- Axe ---
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void AxeAnimation();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Axe")
    UAnimSequence* AxeAsset = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PickaxeAnimation();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickaxe")
    UAnimSequence* PickaxeAsset = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flashlight")
    UAnimSequence* FlashlightAsset = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void FlashlightAnimation();

    UFUNCTION()
    void AnimNotify_Footstep();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    TObjectPtr<USoundBase> FootstepSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    float MinFootstepSpeed = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    TObjectPtr<USoundBase> FootstepStopSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    float StopSpeedThreshold = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    float WasMovingThreshold = 150.f;

    float PrevSpeed2D = 0.f;

    // Jump cue (player only)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    TObjectPtr<USoundBase> JumpSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Footsteps")
    TObjectPtr<USoundBase> LandSound = nullptr;

    bool bWasInAirPrev = false;


	// --- Zombie Attack ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
    UAnimSequence* ZombieAttackAsset = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void ZombieAttackAnimation();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack")
    bool bIsZombieAttacking = false;

    UPROPERTY(BlueprintReadOnly, Category = "Zombie")
    float ZombieSpeedRatio = 1.0f;


};
