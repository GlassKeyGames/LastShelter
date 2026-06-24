// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Results.generated.h"

class UButtonWithText;
class UMyGameInstance;
class UWidget;
class UWidgetSwitcher;

UCLASS()
class END2508_API UResults : public UUserWidget
{
	GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    // Custom buttons
    UPROPERTY(meta = (BindWidget))
    UButtonWithText* RestartButton;

    UPROPERTY(meta = (BindWidget))
    UButtonWithText* MenuButton;

    UPROPERTY(meta = (BindWidget))
    UWidget* ButtonArea;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* ResultsSwitch;

    // GameInstance reference
    UPROPERTY()
    UMyGameInstance* GameInstanceRef;

    // Click handlers
    UFUNCTION()
    void HandleRestartClicked();

    UFUNCTION()
    void HandleMenuClicked();

    // delayed actions (so sound can play)
    UFUNCTION()
    void DoRestart();

    UFUNCTION()
    void DoMenu();

public:
    UFUNCTION(BlueprintCallable)
    void SetWin();

    UFUNCTION(BlueprintCallable)
    void SetLose();

    UFUNCTION()
    void LoadMainMenu();

    FTimerHandle ReturnToMenuTimerHandle;
    FTimerHandle RestartTimerHandle;
    FTimerHandle MenuTimerHandle;
   
};
