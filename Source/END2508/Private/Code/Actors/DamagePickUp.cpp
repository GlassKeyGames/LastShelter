// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/DamagePickUp.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ADamagePickUp::ADamagePickUp()
{
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	// Transform (Location, Rotation, Scale) as seen in the screenshot
	ParticleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	ParticleSystem->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.25f));
	ParticleSystem->SetRelativeRotation(FRotator::ZeroRotator);

	// Particle system asset
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire"));
	if (ParticleAsset.Succeeded())
	{
		ParticleSystem->SetTemplate(ParticleAsset.Object);
	}
}

void ADamagePickUp::HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult)
{
	Super::HandlePickUp_Implementation(OtherActor, SweepResult);

	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
}
