// Fill out your copyright notice in the Description page of Project Settings.


#include "Examples/ExampleFunctions.h"
#include "../END2508.h"

// Sets default values
AExampleFunctions::AExampleFunctions()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExampleFunctions::BeginPlay()
{
	Super::BeginPlay();

	BlueprintCallable();
	BlueprintImplementableEvent();

	UE_LOG(Game, Error, TEXT("In Begin Play: "));
	BlueprintNativeEvent();
	UE_LOG(Game, Log, TEXT("Just called Blueprint Native Event from C++"));
	BlueprintNativeEvent_Implementation();
	UE_LOG(Game, Log, TEXT("Just Called Blueprint Native Event _Implementation from C++"));
}

// Called every frame
void AExampleFunctions::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExampleFunctions::BlueprintCallable()
{
	UE_LOG(Game, Log, TEXT("Blueprint Callable"));
}

float AExampleFunctions::PureFunction() const
{
	return 3.14f;
}

//bool AExampleFunctions::MultipleReturnValues(AActor* Actor1, FRotator Rotation, AActor*& Actor2, int32& Index)
//{
//	return false;
//}

//Blueprint Native Event - Declare in C++, Define in Blueprint and in C++, 
// but have to add _Implementation to the name of the C++ Definition
void AExampleFunctions::BlueprintNativeEvent_Implementation()
{
	UE_LOG(Game, Warning, TEXT("in C++ Blueprint Native Event"));
}

//Cannot Define Implementable Event in C++ it will throw an error
//void AExampleFunctions::BlueprintImplementableEvent()
//{
//	UE_LOG(Game, Log, TEXT("Blueprint Implementable Event"));
//}

