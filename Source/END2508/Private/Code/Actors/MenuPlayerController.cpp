// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/MenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	if (MenuClass)
	{
		MenuInstance = CreateWidget<UUserWidget>(this, MenuClass);
		if (MenuInstance)
		{
			MenuInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways); 
			

			SetInputMode(InputMode);
		}
	}
}