// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/ButtonBuildingSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UButtonBuildingSlot::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (Image && IconTexture)
    {
        Image->SetBrushFromTexture(IconTexture);
    }

}

void UButtonBuildingSlot::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UButtonBuildingSlot::HandleSlotClicked);
        Button->OnHovered.AddDynamic(this, &UButtonBuildingSlot::HandleSlotHovered);
        Button->OnUnhovered.AddDynamic(this, &UButtonBuildingSlot::HandleSlotUnhovered);
    }
}

void UButtonBuildingSlot::HandleSlotClicked()
{
    OnSlotClicked.Broadcast(BuildMesh, BuildDisplayName, WoodCost, StoneCost);
}

void UButtonBuildingSlot::HandleSlotHovered()
{
    OnSlotHovered.Broadcast(BuildDisplayName, BuildDescription, WoodCost, StoneCost);
}


void UButtonBuildingSlot::HandleSlotUnhovered()
{

}
