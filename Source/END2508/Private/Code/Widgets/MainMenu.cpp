// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/MainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Code/MyGameInstance.h"
#include "Both/ButtonWithText.h"
#include "Code/Widgets/SettingsMenu.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button clicks 
	if (PlayGameButton)
	{
		PlayGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnPlayClicked); 
	}

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked); 
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenu::OnSettingsClicked);
	}
}

void UMainMenu::OnPlayClicked()
{

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PlayTimerHandle,
			this,
			&UMainMenu::DoPlay,
			0.25f,   // delay so sound is audible
			false
		);
	}
}

void UMainMenu::DoPlay()
{
	if (UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance()))
	{
		MyGI->LoadFirstLevel();
	}
}

void UMainMenu::OnQuitClicked()
{

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			QuitTimerHandle,
			this,
			&UMainMenu::DoQuit,
			0.25f,
			false
		);
	}

}

void UMainMenu::OnSettingsClicked()
{
	if (!SettingsMenuInstance && SettingsMenuClass)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			SettingsMenuInstance = CreateWidget<USettingsMenu>(PC, SettingsMenuClass);
		}
	}

	if (SettingsMenuInstance)
	{
		SettingsMenuInstance->SetPreviousMenu(this);
		RemoveFromParent();
		SettingsMenuInstance->AddToViewport();
	}
}

void UMainMenu::DoQuit()
{
	if (UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance()))
	{
		MyGI->QuitTheGame();
	}
}