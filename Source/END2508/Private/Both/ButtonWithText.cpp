// Fill out your copyright notice in the Description page of Project Settings.


#include "Both/ButtonWithText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UButtonWithText::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Information)
	{
		Information->SetText(Info);
	}

}


