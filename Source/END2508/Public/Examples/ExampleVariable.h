// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExampleVariable.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateType, AActor*, otherActor);

UCLASS()
class END2508_API AExampleVariable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExampleVariable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables|ReadOnly")
	bool VisibleAnywhere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Variables|ReadOnly")
	uint8 VisibleDefaultsOnly = 8;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Variables|ReadOnly")
	int32 VisibleInstanceOnly;

	UPROPERTY(VisibleInstanceOnly, Category = "Variables|ReadOnly|KindOf")
	int64 VisibleInstanceOnlyOdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|ReadWrite")
	float EditAnywhere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables|ReadWrite")
	FName EditDefaultsOnly;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Variables|ReadWrite")
	FString EditInstanceOnly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|ReadWrite|SortOf")
	FText EditDefaultsOnlyOdd;

	UPROPERTY(EditInstanceOnly, Category = "Variables|ReadWrite|SortOf")
	FVector EditInstanceOnlyOdd;

	//Class Reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Varaibles|ClassReference")
	TSubclassOf<AActor> ClassType;

	//Create a Varaiable of the Delegate/ Event Dispatcher
	//Must have a category to see the delegate in the my blueprint panel
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Variable")
	FDelegateType OnDelegateInstance;

private:
	UFUNCTION()
	void ExampleBoundFunction(AActor* Other);

	UFUNCTION()
	void AnotherExampleBoundFunction(AActor* Other);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Varaibles|Array")
	TArray<int32> ValueArray;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Varaibles|Array")
	TArray<UObject*> PointerArray;

};
