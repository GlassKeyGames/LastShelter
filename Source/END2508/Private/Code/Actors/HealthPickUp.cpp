// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/HealthPickUp.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Code/Utility/HealthComponent.h"

AHealthPickUp::AHealthPickUp()
{
	
	ParticleSystem->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
	ParticleSystem->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.25f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> HeartFX(TEXT("/Game/StarterContent/Particles/P_Heart_Pulsing"));
	if (HeartFX.Succeeded())
	{
		ParticleSystem->SetTemplate(HeartFX.Object);
	}
}

void AHealthPickUp::HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult)
{
	Super::HandlePickUp_Implementation(OtherActor, SweepResult);

}

void AHealthPickUp::PostPickUp_Implementation()
{
	if (ParticleSystem)
	{
		ParticleSystem->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ParticleSystem->bAutoDestroy = true;
	}

	Destroy();
}