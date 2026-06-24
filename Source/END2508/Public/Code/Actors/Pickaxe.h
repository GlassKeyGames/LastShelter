// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Code/Actors/Axe.h"
#include "Pickaxe.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API APickaxe : public AAxe
{
	GENERATED_BODY()
	
public:
	APickaxe();

	virtual bool Attack() override;
};
