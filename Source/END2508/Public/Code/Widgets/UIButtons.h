// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIButtons.generated.h"

class UButton;
class UBorder;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIButtonClicked);

UCLASS()
class END2508_API UUIButtons : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	/** Fired when this button is clicked (after sounds / highlight) */
	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnUIButtonClicked OnClicked;

protected:
	/** Button in your widget. Either 'Button' or 'BackgroundButton' can be used. */
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Button = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BackgroundButton = nullptr;

	/** Optional border used to show hover highlight */
	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* HighlightBorder = nullptr;

	/** Sounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Sound")
	USoundBase* HoverSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Sound")
	USoundBase* ClickSound = nullptr;

	/** Colors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Visual")
	FLinearColor HoverColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Visual")
	FLinearColor NormalColor = FLinearColor::Transparent;

	UButton* CachedButton = nullptr;

	UFUNCTION()
	virtual void HandleHovered();

	UFUNCTION()
	virtual void HandleUnhovered();

	UFUNCTION()
	virtual void HandleClicked();
};
