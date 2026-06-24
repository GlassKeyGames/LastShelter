// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Widgets/ResourcesAmountBox.h"
#include "Components/TextBlock.h"

void UResourcesAmountBox::SetResourceName(const FText& InName)
{
	if (Resource)
	{
		Resource->SetText(InName);
	}
}

void UResourcesAmountBox::SetResourceValue(int32 InValue)
{
	if (Value)
	{
		Value->SetText(FText::AsNumber(InValue));
	}
}

void UResourcesAmountBox::SetResourceData(const FText& InName, int32 InValue)
{
	SetResourceName(InName);
	SetResourceValue(InValue);
}

