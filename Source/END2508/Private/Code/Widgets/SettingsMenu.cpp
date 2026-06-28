// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/SettingsMenu.h"
#include "Both/ButtonWithText.h"

void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsMenu::HandleBackClicked);
	}
}

void USettingsMenu::HandleBackClicked()
{

	RemoveFromParent();

	if (PreviousMenu)
	{
		PreviousMenu->AddToViewport();
	}
}

void USettingsMenu::SetPreviousMenu(UUserWidget* InPreviousMenu)
{
	PreviousMenu = InPreviousMenu;
}