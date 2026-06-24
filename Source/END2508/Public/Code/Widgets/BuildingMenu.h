// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Code/Widgets/UIButtons.h"
#include "Components/TextBlock.h"
#include "BuildingMenu.generated.h"

class UScrollBox;
class UWrapBox;
class UButton;
class UButtonBuildingSlot;
class UBuildComponent;
class UStaticMesh;
class UTexture2D;
class UButtonWithImage;

// entry in the build list (mesh + label + icon) 
USTRUCT(BlueprintType)
struct FBuildOption
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WoodCost = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StoneCost = 0;

};

UCLASS()
class END2508_API UBuildingMenu : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    // ScrollBox to hold all slot widgets 
    UPROPERTY(meta = (BindWidget))
    UScrollBox* BuildScrollBox = nullptr;

    UPROPERTY(meta = (BindWidget))
    UWrapBox* BuildingSlots = nullptr;

    // Slot widget class (WBP_ButtonBuildingSlot) 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    TSubclassOf<UButtonBuildingSlot> SlotWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UButtonWithImage* DestroyButton = nullptr;

    UPROPERTY(meta = (BindWidget))
    UUIButtons* CloseButton;

    // INFO BOX: matches your image
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameTxt = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionTxt = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RequirementsTxt = nullptr;

    // Which build component we’re talking to
    UPROPERTY(BlueprintReadWrite, Category = "Build")
    UBuildComponent* BuildComponentRef = nullptr;

    // List of options to populate into the scroll box 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    TArray<FBuildOption> BuildOptions;

    // Call this right after creating the menu 
    UFUNCTION(BlueprintCallable, Category = "Build")
    void InitializeMenu(UBuildComponent* InBuildComponent);

protected:

    UFUNCTION()
    void HandleDestroyClicked();

    void PopulateSlots();

    UFUNCTION()
    void HandleSlotClicked(UStaticMesh* Mesh, FText Label, int32 WoodCost, int32 StoneCost);

    UFUNCTION()
    void HandleSlotHovered(FText Name, FText Description, int32 WoodCost, int32 StoneCost);

    UFUNCTION()
    void HandleCloseClicked();

    UFUNCTION()
    void HandleDestroyHovered(FText Name, FText Description, FText Requirements);

    UFUNCTION()
    void HandleDestroyUnhovered();
};
