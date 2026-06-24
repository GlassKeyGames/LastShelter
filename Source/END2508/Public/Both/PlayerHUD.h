// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" 
#include "Code/Widgets/ResourcesAmountBox.h"
#include "Code/Widgets/InventorySlot.h"
#include "PlayerHUD.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;
class UInventorySlot;
class UTexture2D;

UCLASS()
class END2508_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxAmmo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;

	UPROPERTY(meta = (BindWidget))
	UWidget* InteractButtonText;



	UFUNCTION(BlueprintCallable)
	void SetAmmo(float Current, float Max);

	UFUNCTION(BlueprintCallable)
	void SetHealth(float Percent);

	UFUNCTION(BlueprintCallable)
	void SetReticleColor(bool bTargetingSpawner);

	UFUNCTION(BlueprintCallable)
	void SetInteractTextVisible(bool bVisible, const FText& Text);



	// Inventory slots (bind to the 4 child widgets you placed)
	UPROPERTY(meta = (BindWidgetOptional))
	UInventorySlot* InventorySlot1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UInventorySlot* InventorySlot2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UInventorySlot* InventorySlot3 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UInventorySlot* InventorySlot4 = nullptr;

	// Helper to set icon
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventorySlotIcon(int32 Index, UTexture2D* Icon);

	// Helper to highlight active slot
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetActiveInventorySlot(int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Slot1Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Slot2Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Slot3Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Slot4Icon = nullptr;



	UPROPERTY(meta = (BindWidgetOptional))
	UResourcesAmountBox* WoodCollectableTxt = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UResourcesAmountBox* StoneCollectableTxt = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetWood(int32 NewWood);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetStone(int32 NewStone);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetResources(int32 NewWood, int32 NewStone);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveText;

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void SetObjectiveText(const FText& NewText);
};
