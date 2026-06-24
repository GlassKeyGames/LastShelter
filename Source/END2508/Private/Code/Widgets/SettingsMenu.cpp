// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/SettingsMenu.h"
#include "Both/ButtonWithText.h"

void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("SettingsMenu NativeConstruct"));

	if (BackButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("BackButton bound"));
		BackButton->OnClicked.AddDynamic(this, &USettingsMenu::HandleBackClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BackButton is NULL"));
	}
}

void USettingsMenu::HandleBackClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Back clicked"));

	RemoveFromParent();

	if (PreviousMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("Returning to previous menu"));
		PreviousMenu->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PreviousMenu is NULL"));
	}
}

void USettingsMenu::SetPreviousMenu(UUserWidget* InPreviousMenu)
{
	PreviousMenu = InPreviousMenu;
}