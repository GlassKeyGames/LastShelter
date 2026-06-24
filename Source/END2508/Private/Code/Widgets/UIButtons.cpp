// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/UIButtons.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"

void UUIButtons::NativeConstruct()
{
	Super::NativeConstruct();

	CachedButton = Button ? Button : BackgroundButton;
	if (!CachedButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUIButtons: No Button bound on %s"), *GetName());
		return;
	}

	if (!CachedButton->OnHovered.IsAlreadyBound(this, &UUIButtons::HandleHovered))
	{
		CachedButton->OnHovered.AddDynamic(this, &UUIButtons::HandleHovered);
	}

	if (!CachedButton->OnUnhovered.IsAlreadyBound(this, &UUIButtons::HandleUnhovered))
	{
		CachedButton->OnUnhovered.AddDynamic(this, &UUIButtons::HandleUnhovered);
	}

	if (!CachedButton->OnClicked.IsAlreadyBound(this, &UUIButtons::HandleClicked))
	{
		CachedButton->OnClicked.AddDynamic(this, &UUIButtons::HandleClicked);
	}

	if (HighlightBorder)
	{
		HighlightBorder->SetBrushColor(NormalColor);
	}
}

void UUIButtons::HandleHovered()
{
	if (HighlightBorder)
	{
		HighlightBorder->SetBrushColor(HoverColor);
	}

	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}

void UUIButtons::HandleUnhovered()
{
	if (HighlightBorder)
	{
		HighlightBorder->SetBrushColor(NormalColor);
	}
}

void UUIButtons::HandleClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	OnClicked.Broadcast();
}
