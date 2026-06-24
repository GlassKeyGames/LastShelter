// Fill out your copyright notice in the Description page of Project Settings.

#include "Code/Widgets/ButtonWithImage.h"
#include "Components/Image.h"

void UButtonWithImage::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (IconImage && IconTexture)
    {
        IconImage->SetBrushFromTexture(IconTexture);
    }
}

void UButtonWithImage::HandleHovered()
{
    // Keep all the default behaviour from UUIButtons (sound, border color, etc.)
    Super::HandleHovered();

    // Then update the info box via delegate
    OnImageSlotHovered.Broadcast(HoverName, HoverDescription, HoverRequirements);
}

void UButtonWithImage::HandleUnhovered()
{
    Super::HandleUnhovered();

    OnImageSlotUnhovered.Broadcast();
}


