// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMyGameInstance::LoadFirstLevel()
{
	if (GameLevels.IsValidIndex(FirstLevelIndex))
	{
		CurrentLevelIndex = FirstLevelIndex;
		FName LevelName = GameLevels[FirstLevelIndex];

		UGameplayStatics::OpenLevel(this, LevelName);
	}
}

void UMyGameInstance::LoadCurrentLevel()
{
	if (GameLevels.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(this, GameLevels[CurrentLevelIndex]);
	}
}

void UMyGameInstance::LoadMainMenu()
{
	const FName MainMenuLevel = "CodeMainMenu";  
	UGameplayStatics::OpenLevel(this, MainMenuLevel);
}

void UMyGameInstance::QuitTheGame()
{
	
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
		}
	}
}
