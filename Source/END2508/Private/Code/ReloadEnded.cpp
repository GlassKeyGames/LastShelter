// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/ReloadEnded.h"
#include "GameFramework/Actor.h"
#include "Code/Actors/Rifle.h"
#include "Code/Actors/BasePlayer.h"

void UReloadEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (ABasePlayer* Player = Cast<ABasePlayer>(Owner))
	{
		if (ARifle* R = Player->Weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("? AnimNotify: Player reload complete."));
			R->ResetAction();
		}
	}
}