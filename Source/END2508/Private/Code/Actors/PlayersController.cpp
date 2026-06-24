// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/PlayersController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void APlayersController::BeginPlay()
{
    Super::BeginPlay();

    // Add the mapping context so IA_Pause actually fires
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (GameplayMappingContext)
            {
                Subsystem->AddMappingContext(GameplayMappingContext, 0);
            }
        }
    }
}

void APlayersController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput =
        Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (IA_Pause)
        {
            EnhancedInput->BindAction(
                IA_Pause,
                ETriggerEvent::Started,      // or Completed, depending on your IA
                this,
                &APlayersController::OnPauseTriggered
            );
        }
    }
}

void APlayersController::OnPauseTriggered(const FInputActionValue& Value)
{
    if (!bIsPaused)
    {
        ShowPauseMenu();
    }
    else
    {
        HidePauseMenu();
    }
}

void APlayersController::ShowPauseMenu()
{
    if (!PauseMenuInstance && PauseMenuClass)
    {
        PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
    }

    if (!PauseMenuInstance) return;

    PauseMenuInstance->AddToViewport();

    bShowMouseCursor = true;

    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    SetInputMode(Mode);

    SetPause(true);
    bIsPaused = true;
}

void APlayersController::HidePauseMenu()
{

    if (PauseMenuInstance)
    {
        PauseMenuInstance->RemoveFromParent();
    }

    bShowMouseCursor = false;

    FInputModeGameOnly Mode;
    SetInputMode(Mode);

    SetPause(false);
    bIsPaused = false;
}
