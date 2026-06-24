// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Both/ButtonWithText.h"
#include "MainMenu.generated.h"

class USettingsMenu;

UCLASS()
class END2508_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
protected:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* PlayGameButton;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* QuitGameButton;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* SettingsButton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<USettingsMenu> SettingsMenuClass;

    UPROPERTY()
    USettingsMenu* SettingsMenuInstance = nullptr;

    UFUNCTION()
    void OnPlayClicked();

    UFUNCTION()
    void OnQuitClicked();

    UFUNCTION()
    void OnSettingsClicked();

    // Delayed actions so click sound can play
    UFUNCTION()
    void DoPlay();

    UFUNCTION()
    void DoQuit();

    FTimerHandle PlayTimerHandle;
    FTimerHandle QuitTimerHandle;
};
