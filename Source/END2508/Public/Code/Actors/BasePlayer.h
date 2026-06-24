// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "InputMappingContext.h"
#include "Code/Actors/Axe.h"
#include "Code/EquipedState.h"
#include "Code/Actors/Pickaxe.h"
#include "Code/Actors/Flashlight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BasePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPlayerHUD;
class APawn;
class UAIPerceptionStimuliSourceComponent;
class ARifle;
class UTexture2D;
class APickUps;
class ADoor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLost);

USTRUCT(BlueprintType)
struct FHotbarEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ItemClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bOccupied = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipState EquipType = EEquipState::Unarmed;

	// optional later: spawned instance
	UPROPERTY(Transient)
	AActor* SpawnedActor = nullptr;
};

UCLASS()
class END2508_API ABasePlayer : public ABaseCharacter
{
	GENERATED_BODY()
public:

	
	ABasePlayer(const FObjectInitializer& ObjectInitializer);




	// --- Camera ---
	UFUNCTION()
	void ToggleCameraView();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bIsFirstPerson = false;





	// --- Movement ---
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);




	// Input Actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;





	//UPDATE HEALTH
	virtual void UpdateHealthHUD(float Current, float Max) override;



	//LOST THE GAME
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnPlayerLost OnPlayerLost;



	//Ammo
	void AddMaxAmmo(int32 Amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxAmmo = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 CurrentAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ClipSize = 30;

	UFUNCTION()
	void UpdateAmmoHUD();

	UPlayerHUD* GetHUDWidget() const { return HUDWidget; }


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Handle jump input (called when jump action is triggered) 
	virtual void Jump() override;
	// Handle stop jumping input (called when jump action is released) 
	virtual void StopJumping() override;


	// --- UI ---
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUD> HUDWidgetClass;

	UPROPERTY(Transient)
	UPlayerHUD* HUDWidget = nullptr;





	// --- AI ---
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<APawn> AgentToSpawn;





	// --- Input ---
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleViewAction = nullptr;






	// --- Components ---
	UPROPERTY()
	ARifle* PlayerRifle = nullptr;

	UFUNCTION()
	void HandleReloadInput();





	// --- Game State ---
	UFUNCTION()
	void PlayerLost();

	UFUNCTION(BlueprintCallable)
	void PlayerWin();

	UFUNCTION()
	void OnInteract();

	UFUNCTION()
	void HandleBuildKey();






	// ---- Inventory Hotbar Input Actions ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hotbar")
	TArray<FHotbarEntry> Hotbar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hotbar")
	int32 ActiveHotbarIndex = 0;

	// Adds pickup into first empty slot + updates HUD icon
	UFUNCTION()
	bool AddToFirstAvailableSlot(TSubclassOf<AActor> ItemClass, UTexture2D* Icon, EEquipState EquipType, int32& OutIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Hotbar1Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Hotbar2Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Hotbar3Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Hotbar4Action = nullptr;

	void EquipHotbarSlot(int32 Index);

	UFUNCTION()
	void Hotbar1Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Hotbar2Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Hotbar3Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Hotbar4Pressed(const FInputActionValue& Value);




	// JUMPING
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(BlueprintReadWrite, Category = "Jump")
	bool bJustJumped = false;




	// WEAPONS
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AAxe> AxeClass;

	UPROPERTY()
	AAxe* Axe = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bHoldingAxe = false;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	EEquipState EquipState = EEquipState::Unarmed;

	UFUNCTION()
	void DoAxeAttack();

	UFUNCTION(BlueprintCallable)
	void OnAxeHitNotify(); 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Axe|Combat")
	float AxeAttackCooldown = 0.45f;

	UPROPERTY(BlueprintReadOnly, Category = "Axe|Combat")
	bool bCanAxeAttack = true;

	FTimerHandle AxeCooldownHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<APickaxe> PickaxeClass;

	UPROPERTY()
	APickaxe* Pickaxe = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickaxe|Animation")
	UAnimSequence* PickaxeAsset = nullptr;

	UFUNCTION()
	void DoPickaxeAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<AFlashlight> FlashlightClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AFlashlight* Flashlight = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName FlashlightAttachSocket = TEXT("WeaponSocket");

	// Flashlight use
	UFUNCTION()
	void ToggleFlashlight();

	UFUNCTION()
	void DoFlashlightUse();



	// RESOURCES
	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 WoodCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 StoneCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void AddWood(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void AddStone(int32 Amount);

	void UpdateResourceHUD();




	// PICKUPS
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetCurrentPickup(APickUps* NewPickup);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	APickUps* GetCurrentPickup() const { return CurrentPickup; }

	UPROPERTY()
	APickUps* CurrentPickup = nullptr;





	// Door interaction
	// The door we are currently looking at
	UPROPERTY()
	ADoor* CurrentDoorActor = nullptr;

	// How far the door swings when opened
	UPROPERTY(EditAnywhere, Category = "Door")
	float DoorOpenYawOffset = 90.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 QuestStep = 0;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void UpdateQuestText();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AdvanceQuestStep();

	UFUNCTION()
	void HandlePickupQuest(EEquipState PickedUpType);

	UFUNCTION()
	void HandleWoodQuest();

	UFUNCTION()
	void HandleStoneQuest();

	UFUNCTION()
	void HandleBuildQuest();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void RegisterPlacedBuildByType(bool bFoundation, bool bWall, bool bDoorFrame, bool bCeiling);

	UFUNCTION(BlueprintCallable)
	void HandleAbandonedBuildingQuest();

	UFUNCTION(BlueprintCallable)
	void HandleGunQuest();

	UFUNCTION(BlueprintCallable)
	void HandleAmmoQuest();

	UFUNCTION(BlueprintCallable)
	void ForceFirstPerson(bool bEnable);


private:
	void InputAxisMoveForward(float Value);
	void InputAxisMoveSideways(float Value);
	void InputAxisLookUp(float Value);
	void AttackInput();
	void TellAllAgentsToAttack();

};
