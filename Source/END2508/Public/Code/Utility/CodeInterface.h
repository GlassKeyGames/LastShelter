// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CodeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCodeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class END2508_API ICodeInterface
{
	GENERATED_BODY()

public:

	virtual void EnemyAttack() = 0;
	virtual void HandleActionFinished() = 0;
	virtual void EnemyReload() = 0;
	virtual bool CanPickupHealth() = 0;
	
};
