// Fill out your copyright notice in the Description page of Project Settings.


#include "Both/PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Code/Actors/BasePlayer.h"
#include "Code/Widgets/ResourcesAmountBox.h"
#include "Code/Widgets/InventorySlot.h"
#include "Components/TextBlock.h"

void UPlayerHUD::SetInteractTextVisible(bool bVisible, const FText& Text)
{
    // Update the description text (Build Foundation / Open / etc.)
    if (InteractText)
    {
        // Only overwrite if caller passed something
        if (!Text.IsEmpty())
        {
            InteractText->SetText(Text);
        }

        InteractText->SetVisibility(
            bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden
        );
    }

    // Toggle the "E" label too, if we have one
    if (InteractButtonText)
    {
        InteractButtonText->SetVisibility(
            bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden
        );
    }

    // Apply the images to the slots
    if (Slot1Icon) SetInventorySlotIcon(0, Slot1Icon);
    if (Slot2Icon) SetInventorySlotIcon(1, Slot2Icon);
    if (Slot3Icon) SetInventorySlotIcon(2, Slot3Icon);
    if (Slot4Icon) SetInventorySlotIcon(3, Slot4Icon);


}

void UPlayerHUD::SetInventorySlotIcon(int32 Index, UTexture2D* Icon)
{
    UInventorySlot* TargetSlot = nullptr;

    switch (Index)
    {
    case 0: TargetSlot = InventorySlot1; break;
    case 1: TargetSlot = InventorySlot2; break;
    case 2: TargetSlot = InventorySlot3; break;
    case 3: TargetSlot = InventorySlot4; break;
    default: return;
    }

    if (TargetSlot)
    {
        TargetSlot->SetIcon(Icon);
    }
}

void UPlayerHUD::SetActiveInventorySlot(int32 Index)
{
    TArray<UInventorySlot*> Slots;
    Slots.Add(InventorySlot1);
    Slots.Add(InventorySlot2);
    Slots.Add(InventorySlot3);
    Slots.Add(InventorySlot4);

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i])
        {
            Slots[i]->SetSelected(i == Index);
        }
    }
}

void UPlayerHUD::SetWood(int32 NewWood)
{
    if (WoodCollectableTxt)
    {
        WoodCollectableTxt->SetResourceData(FText::FromString(TEXT("Wood")), NewWood);
    }
}

void UPlayerHUD::SetStone(int32 NewStone)
{
    if (StoneCollectableTxt)
    {
        StoneCollectableTxt->SetResourceData(FText::FromString(TEXT("Stone")), NewStone);
    }
}

void UPlayerHUD::SetResources(int32 NewWood, int32 NewStone)
{
    SetWood(NewWood);
    SetStone(NewStone);
}

void UPlayerHUD::SetAmmo(float Current, float Max)
{

    if (CurrentAmmo)
    {
        CurrentAmmo->SetText(FText::AsNumber(FMath::FloorToInt(Current)));
    }

    if (MaxAmmo)
    {
        MaxAmmo->SetText(FText::AsNumber(FMath::FloorToInt(Max)));
    }
}

void UPlayerHUD::SetHealth(float Percent)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(Percent);
    }
}

void UPlayerHUD::SetReticleColor(bool bTargetingSpawner)
{
    if (!Crosshair) return;

    FLinearColor NewColor = bTargetingSpawner ? FLinearColor::Red : FLinearColor::White;
    Crosshair->SetColorAndOpacity(NewColor);
}

void UPlayerHUD::SetObjectiveText(const FText& NewText)
{
    if (ObjectiveText)
    {
        ObjectiveText->SetText(NewText);
    }
}