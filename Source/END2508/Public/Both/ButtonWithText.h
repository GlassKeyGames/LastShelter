// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Widgets/UIButtons.h"
#include "ButtonWithText.generated.h"

class UButton;
class UTextBlock;


UCLASS()
class END2508_API UButtonWithText : public UUIButtons
{
	GENERATED_BODY()
public:

	// Called when the widget is constructed
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Information;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FText Info;


};
