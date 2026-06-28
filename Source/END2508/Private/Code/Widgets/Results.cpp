// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/Results.h"
#include "Components/Button.h"
#include "Code/MyGameInstance.h"
#include "Both/ButtonWithText.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "TimerManager.h"

void UResults::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstanceRef = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UResults::HandleRestartClicked);
	}

	if (MenuButton)
	{
		MenuButton->OnClicked.AddDynamic(this, &UResults::HandleMenuClicked);
	}
}

void UResults::HandleRestartClicked()
{
	// ClickSound already played by UUIButtons
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RestartTimerHandle,
			this,
			&UResults::DoRestart,
			0.25f,
			false
		);
	}
}

void UResults::HandleMenuClicked()
{
	// ClickSound already played by UUIButtons
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			MenuTimerHandle,
			this,
			&UResults::DoMenu,
			0.25f,
			false
		);
	}
}

void UResults::DoRestart()
{
	if (GameInstanceRef)
	{
		GameInstanceRef->LoadCurrentLevel();
	}
}

void UResults::DoMenu()
{
	if (GameInstanceRef)
	{
		GameInstanceRef->LoadMainMenu();
	}
}

void UResults::SetWin()
{
	if (ButtonArea)
	{
		ButtonArea->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ResultsSwitch)
	{
		ResultsSwitch->SetActiveWidgetIndex(1); // "You Win!" index
	}

	// Auto return to menu after 3 seconds (NO fake click)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ReturnToMenuTimerHandle,
			this,
			&UResults::LoadMainMenu,
			3.0f,
			false
		);
	}
}

void UResults::SetLose()
{
	if (ButtonArea)
	{
		ButtonArea->SetVisibility(ESlateVisibility::Visible);
	}

	if (ResultsSwitch)
	{
		ResultsSwitch->SetActiveWidgetIndex(0); // Index 0 = "You Lose!" 
	}

	UE_LOG(LogTemp, Warning, TEXT("SetLose triggered — displaying lose screen"));
}

void UResults::LoadMainMenu()
{
	DoMenu();
}