// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/BuildingMenu.h"
#include "Code/Widgets/ButtonBuildingSlot.h"
#include "Components/ScrollBox.h"
#include "Code/Widgets/ButtonWithImage.h"
#include "Components/WrapBox.h" 
#include "BuildComponent.h"

void UBuildingMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (DestroyButton)
    {
        DestroyButton->OnClicked.AddDynamic(
            this,
            &UBuildingMenu::HandleDestroyClicked
        );

        DestroyButton->OnImageSlotHovered.AddDynamic(
            this,
            &UBuildingMenu::HandleDestroyHovered
        );

        DestroyButton->OnImageSlotUnhovered.AddDynamic(
            this,
            &UBuildingMenu::HandleDestroyUnhovered
        );
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UBuildingMenu::HandleCloseClicked);
    }
   
}

void UBuildingMenu::InitializeMenu(UBuildComponent* InBuildComponent)
{
    BuildComponentRef = InBuildComponent;
    PopulateSlots();
}

void UBuildingMenu::HandleDestroyClicked()
{
    if (BuildComponentRef)
    {
        // Put BuildComponent into a special destroy mode
        BuildComponentRef->StartDestroyMode();
    }

    // Give control back to game
    if (APlayerController* PC = GetOwningPlayer())
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = false;
    }

    // Close the menu
    RemoveFromParent();
}

void UBuildingMenu::PopulateSlots()
{
    if (!BuildingSlots || !SlotWidgetClass)
    {
        return;
    }

    BuildingSlots->ClearChildren();   // clear the WrapBox children

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (const FBuildOption& Option : BuildOptions)
    {
        if (!Option.Mesh) continue;

        UE_LOG(LogTemp, Warning, TEXT("PopulateSlots -> %s | Wood=%d | Stone=%d"),
            *Option.Label.ToString(), Option.WoodCost, Option.StoneCost);

        UButtonBuildingSlot* SlotWidget =
            CreateWidget<UButtonBuildingSlot>(World, SlotWidgetClass);

        if (!SlotWidget) continue;

        SlotWidget->BuildMesh = Option.Mesh;
        SlotWidget->BuildDisplayName = Option.Label;
        SlotWidget->IconTexture = Option.Icon;
        SlotWidget->BuildDescription = Option.Description;
        SlotWidget->WoodCost = Option.WoodCost;
        SlotWidget->StoneCost = Option.StoneCost;

        SlotWidget->OnSlotClicked.AddDynamic(
            this, &UBuildingMenu::HandleSlotClicked);

        SlotWidget->OnSlotHovered.AddDynamic(
            this, &UBuildingMenu::HandleSlotHovered);

        BuildingSlots->AddChildToWrapBox(SlotWidget);
    }
}

void UBuildingMenu::HandleSlotClicked(UStaticMesh* Mesh, FText Label, int32 WoodCost, int32 StoneCost)
{
    UE_LOG(LogTemp, Warning, TEXT("Menu Clicked -> %s | WoodCost=%d | StoneCost=%d"),
        *Label.ToString(), WoodCost, StoneCost);

    if (BuildComponentRef && Mesh)
    {
        BuildComponentRef->SetCurrentBuild(Mesh, Label, WoodCost, StoneCost);
        BuildComponentRef->StartBuildMode();
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = false;
    }

    RemoveFromParent();
}

void UBuildingMenu::HandleSlotHovered(FText Name, FText Description, int32 WoodCost, int32 StoneCost)
{
    if (NameTxt)
    {
        NameTxt->SetText(Name);
    }

    if (DescriptionTxt)
    {
        DescriptionTxt->SetText(Description);
    }

    if (RequirementsTxt)
    {
        FString CostString;

        if (WoodCost > 0 && StoneCost > 0)
        {
            CostString = FString::Printf(TEXT("%d Wood, %d Stone"), WoodCost, StoneCost);
        }
        else if (WoodCost > 0)
        {
            CostString = FString::Printf(TEXT("%d Wood"), WoodCost);
        }
        else if (StoneCost > 0)
        {
            CostString = FString::Printf(TEXT("%d Stone"), StoneCost);
        }
        else
        {
            CostString = TEXT("Free");
        }

        RequirementsTxt->SetText(FText::FromString(CostString));
    }
}

void UBuildingMenu::HandleCloseClicked()
{
    if (BuildComponentRef)
    {
        BuildComponentRef->ExitBuildAndCloseMenu();
    }
}

void UBuildingMenu::HandleDestroyHovered(FText Name, FText Description, FText Requirements)
{
    // You can either use Name/Description/Requirements passed in,
    // or ignore them and hard-code text. Using them keeps it flexible.
    if (NameTxt)
    {
        NameTxt->SetText(Name);
    }
    if (DescriptionTxt)
    {
        DescriptionTxt->SetText(Description);
    }
    if (RequirementsTxt)
    {
        RequirementsTxt->SetText(Requirements);
    }
}

void UBuildingMenu::HandleDestroyUnhovered()
{
    // Optional: clear the info box when leaving destroy
    if (NameTxt)
    {
        NameTxt->SetText(FText::GetEmpty());
    }
    if (DescriptionTxt)
    {
        DescriptionTxt->SetText(FText::GetEmpty());
    }
    if (RequirementsTxt)
    {
        RequirementsTxt->SetText(FText::GetEmpty());
    }
}