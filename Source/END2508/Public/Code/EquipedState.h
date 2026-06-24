// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEquipState : uint8
{
    Unarmed    UMETA(DisplayName = "Unarmed"),
    Rifle      UMETA(DisplayName = "Rifle"),
    Axe        UMETA(DisplayName = "Axe"),
    PickAxe    UMETA(DisplayName = "PickAxe"),
    Flashlight UMETA(DisplayName = "Flashlight")
};

//class END2508_API EquipedState
//{
//public:
	//EquipedState();
	//~EquipedState();
//};
