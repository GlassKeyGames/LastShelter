// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Widgets/UIButtons.h"
#include "ButtonBuildingSlot.generated.h"

class UButton;
class UImage;
class UStaticMesh;
class UTexture2D;

// Delegate to tell the menu what was clicked
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnBuildingSlotClicked,
    UStaticMesh*, Mesh,
    FText, DisplayName,
    int32, WoodCost,
    int32, StoneCost
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnSlotHovered,
    FText, Name,
    FText, Description,
    int32, WoodCost,
    int32, StoneCost
);

UCLASS()
class END2508_API UButtonBuildingSlot : public UUIButtons
{
	GENERATED_BODY()
public:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;


    // This is the Image under the Button
    UPROPERTY(meta = (BindWidget))
    UImage* Image;

    // Icon that shows in the slot
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    UTexture2D* IconTexture = nullptr;

    // Mesh that this slot represents (Foundation, Wall, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    UStaticMesh* BuildMesh = nullptr;

    // Name used for HUD text (ex: "Build Foundation", "Build Wall")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    FText BuildDisplayName;

    // Broadcast to BuildingMenu when this slot is clicked
    UPROPERTY(BlueprintAssignable, Category = "Build")
    FOnBuildingSlotClicked OnSlotClicked;

    UPROPERTY(BlueprintAssignable, Category = "Build")
    FOnSlotHovered OnSlotHovered;

    UPROPERTY(BlueprintReadWrite, Category = "Build")
    FText BuildDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    int32 WoodCost = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    int32 StoneCost = 0;

protected:
    UFUNCTION()
    void HandleSlotClicked();

    UFUNCTION()
    void HandleSlotHovered();

    UFUNCTION()
    void HandleSlotUnhovered();

};
