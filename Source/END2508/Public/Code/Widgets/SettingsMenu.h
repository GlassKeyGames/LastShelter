// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

class UUserWidget;
class UButtonWithText;

UCLASS()
class END2508_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPreviousMenu(UUserWidget* InPreviousMenu);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	UUserWidget* PreviousMenu = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButtonWithText* BackButton = nullptr;

	UFUNCTION()
	void HandleBackClicked();
};
