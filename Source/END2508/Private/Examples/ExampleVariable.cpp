// Fill out your copyright notice in the Description page of Project Settings.


#include "Examples/ExampleVariable.h"
#include "../END2508.h"

// Sets default values
AExampleVariable::AExampleVariable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClassType = AExampleVariable::StaticClass();

	ValueArray.Add(3);
	ValueArray.Add(4);
	PointerArray.Add(this);
	PointerArray.Add(nullptr);
}

// Called when the game starts or when spawned
void AExampleVariable::BeginPlay()
{
	Super::BeginPlay();
	
	//This is how C++ does the bind in Blueprint
	//Params sent into the AddDynamic are the same thinag as created in Blueprint
	//As seen in Base Begin Play
	OnDelegateInstance.AddDynamic(this, &AExampleVariable::ExampleBoundFunction);
	OnDelegateInstance.AddDynamic(this, &AExampleVariable::AnotherExampleBoundFunction);

	//This is how we call the delegate
	OnDelegateInstance.Broadcast(this);
}

// Called every frame
void AExampleVariable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExampleVariable::ExampleBoundFunction(AActor* Other)
{
	UE_LOG(Game, Error, TEXT("Function Called by Delegate"));
}

void AExampleVariable::AnotherExampleBoundFunction(AActor* Other)
{
	UE_LOG(Game, Error, TEXT("Another Function Called by Delegate"));
}

