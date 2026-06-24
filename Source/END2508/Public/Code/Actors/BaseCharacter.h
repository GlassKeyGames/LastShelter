// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Code/Utility/HealthComponent.h"
#include "Components/ChildActorComponent.h"
#include "Code/Actors/Rifle.h" 
#include "Code/Utility/CodeInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class ARifle;
class UCharacterAnimation;
class UHealthComponent;
class UAIPerceptionStimuliSourceComponent;
class UBuildComponent;
class UInputAction;

UCLASS()
class END2508_API ABaseCharacter : public ACharacter, public ICodeInterface
{
	GENERATED_BODY()
	
public:	
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	// Sets default values for this actor's properties
	virtual void BeginPlay() override;

	void DestroyActorComponent(UActorComponent* ComponentToDestroy);

	UPROPERTY(Category = Character, VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UChildActorComponent* WeaponChild;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimationBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionStimuliSourceComponent* StimuliSource;




public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void EnemyAttack() override;
	virtual void HandleActionFinished() override;
	virtual void EnemyReload() override;
	virtual bool CanPickupHealth() override;
	virtual void HandleDeath(float Ratio);

	UPROPERTY()
	ARifle* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocket = TEXT("hand_r");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName AxeAttachSocket = TEXT("hand_rSocket_Axe");

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void AttachWeapons();

	UFUNCTION()
	void OnAttack();

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* FireAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* HitAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* DeathAnim1;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* DeathAnim2;

	UFUNCTION()
	virtual void UpdateHealthHUD(float Current, float Max);

	UFUNCTION(BlueprintCallable)
	virtual void OnPlayerDeath();

	bool bCanFire = true;
	float FireCooldown = 1.0f;
	bool bIsDead = false;

	FTimerHandle FireCooldownHandle;

	UFUNCTION(BlueprintCallable)
	virtual void OnDeathEnded();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* BuildModeAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build", meta = (AllowPrivateAccess = "true"))
	UBuildComponent* BuildComponent = nullptr;

	void BuildMode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* OpenBuildMenuAction = nullptr;
};
