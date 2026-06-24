// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/PauseMenu.h"
#include "Both/ButtonWithText.h" 
#include "Code/Actors/PlayersController.h"     // your controller
#include "Kismet/GameplayStatics.h"
#include "Code/Widgets/SettingsMenu.h"

void UPauseMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::HandleResumeClicked);
    }

    if (MenuButton)
    {
        MenuButton->OnClicked.AddDynamic(this, &UPauseMenu::HandleMenuClicked);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UPauseMenu::HandleSettingsClicked);
    }
}

void UPauseMenu::HandleResumeClicked()
{
    if (APlayersController* PC = Cast<APlayersController>(GetOwningPlayer()))
    {
        PC->HidePauseMenu();        // uses your existing C++ logic
    }
}

void UPauseMenu::HandleMenuClicked()
{
    if (APlayersController* PC = Cast<APlayersController>(GetOwningPlayer()))
    {
        PC->HidePauseMenu();
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("CodeMainMenu")));
}

void UPauseMenu::HandleSettingsClicked()
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
