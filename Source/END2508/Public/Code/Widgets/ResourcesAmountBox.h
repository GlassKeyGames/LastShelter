// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourcesAmountBox.generated.h"

class UTextBlock;

UCLASS()
class END2508_API UResourcesAmountBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Resource = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Value = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetResourceName(const FText& InName);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetResourceValue(int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SetResourceData(const FText& InName, int32 InValue);
};
