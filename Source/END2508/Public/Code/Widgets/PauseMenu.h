// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

class UButtonWithText;
class USettingsMenu;

UCLASS()
class END2508_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    // These names MUST match the widget names in your WBP:
    UPROPERTY(meta = (BindWidget))
    UButtonWithText* ResumeButton;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* MenuButton;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* SettingsButton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<USettingsMenu> SettingsMenuClass;

    UPROPERTY()
    USettingsMenu* SettingsMenuInstance = nullptr;

    UFUNCTION()
    void HandleResumeClicked();

    UFUNCTION()
    void HandleMenuClicked();

    UFUNCTION()
    void HandleSettingsClicked();
};
