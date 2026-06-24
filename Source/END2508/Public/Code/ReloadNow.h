// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ReloadNow.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API UReloadNow : public UAnimNotify
{
	GENERATED_BODY()
public: 
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
