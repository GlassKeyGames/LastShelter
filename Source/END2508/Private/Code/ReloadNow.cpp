// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/ReloadNow.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Actor.h"
#include "Code/Actors/BasePlayer.h"  // Your custom player header
#include "Both/CharacterAnimation.h" // Your custom anim instance header

void UReloadNow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (ABasePlayer* Player = Cast<ABasePlayer>(Owner))
	{
		if (ARifle* R = Player->Weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("? ReloadNow Notify: Calling ReloadAmmo() directly"));
			R->CompleteReload(); // You SHOULD call this here
		}
	}
}
