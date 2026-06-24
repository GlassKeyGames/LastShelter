// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class END2508_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Which hotkey this slot represents (0 = "1", 1 = "2", etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SlotIndex = 0;

    // Set / change the icon from code
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetIcon(UTexture2D* InTexture);

    // Optional: highlight when selected
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSelected(bool bSelected);

protected:
    // Bind these to the widgets inside WBP_InventorySlots
    UPROPERTY(meta = (BindWidget))
    UImage* Icon = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SlotText = nullptr;

};
