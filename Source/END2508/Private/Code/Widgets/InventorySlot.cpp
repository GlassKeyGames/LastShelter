// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();

    if (SlotText)
    {
        SlotText->SetText(FText::AsNumber(SlotIndex + 1));
    }
}

void UInventorySlot::SetIcon(UTexture2D* InTexture)
{
    if (!Icon) return;

    if (InTexture)
    {
        Icon->SetBrushFromTexture(InTexture);
        Icon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        Icon->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventorySlot::SetSelected(bool bSelected)
{
    if (Icon)
    {
        // Cheap highlight: brighter when selected
        Icon->SetOpacity(bSelected ? 1.0f : 0.6f);
    }

    if (SlotText)
    {
        SlotText->SetColorAndOpacity(
            bSelected ? FSlateColor(FLinearColor::Yellow)
            : FSlateColor(FLinearColor::White));
    }
}
