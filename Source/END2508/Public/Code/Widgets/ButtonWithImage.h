// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Widgets/UIButtons.h"
#include "ButtonWithImage.generated.h"

class UButton;
class UImage;

// Hover event: sends Name / Description / Requirements
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnImageSlotHovered,
    FText, Name,
    FText, Description,
    FText, Requirements
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImageSlotUnhovered);


UCLASS()
class END2508_API UButtonWithImage : public UUIButtons
{
	GENERATED_BODY()
	
public:
    virtual void NativePreConstruct() override;

    // Icon image inside the widget
    UPROPERTY(meta = (BindWidget))
    UImage* IconImage;

    // Texture to show on the icon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    UTexture2D* IconTexture;

    // --- Info-box text when this button is hovered ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
    FText HoverName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
    FText HoverDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
    FText HoverRequirements;

    // These mimic the building slot events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnImageSlotHovered OnImageSlotHovered;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnImageSlotUnhovered OnImageSlotUnhovered;

protected:
    // ?? Extend UUIButtons behaviour: sounds + highlight + our broadcast
    virtual void HandleHovered() override;
    virtual void HandleUnhovered() override;


};
