// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildComponent.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class ABaseCharacter;
class UMaterialInterface;
class UBuildingMenu;
class UPlayerHUD;
class USoundBase;
class ADoor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class END2508_API UBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABaseCharacter* PlayerRef = nullptr;

	UPROPERTY()
	UStaticMeshComponent* BuildPreview = nullptr;

	FTransform BuildTransform;
	bool IsBuildModeOn = false;
	
	void BuildCycle();
	void SpawnBuildable();
	void UpdateBuildPreview();

	bool DoOnce = true;
	bool CanPlace = false;

	FTimerHandle BuildCycleHandle;

	UPROPERTY()
	UBuildingMenu* ActiveMenu = nullptr;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleBuildMode();
	void PlaceBuildable();

	UPROPERTY()
	UCameraComponent* Camera = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build|Grid")
	float GridSize = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build|Grid")
	float TileSize = 400.f;

	// Meshes for building types
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build")
	UStaticMesh* FoundationMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
	TArray<UStaticMesh*> OtherFoundationMeshes;

	UPROPERTY(EditAnywhere, Category = "Build")
	UStaticMesh* WallMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
	TArray<UStaticMesh*> OtherWallMeshes;

	UPROPERTY(EditAnywhere, Category = "Build|Frames")
	UStaticMesh* DoorFrameMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Frames")
	TArray<UStaticMesh*> OtherDoorFrameMeshes;


	UPROPERTY(EditAnywhere, Category = "Build|Frames")
	UStaticMesh* WindowFrameMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Frames")
	TArray<UStaticMesh*> OtherWindowFrameMeshes;








	// Inserts (placed into frames)
	UPROPERTY(EditAnywhere, Category = "Build|Door")
	UStaticMesh* DoorMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Door")
	TArray<UStaticMesh*> OtherDoorMeshes;

	UPROPERTY(EditAnywhere, Category = "Build|Window")
	UStaticMesh* WindowMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Window")
	TArray<UStaticMesh*> OtherWindowMeshes;





	bool IsWallMesh(UStaticMesh* Mesh) const;

	// how far to raise walls when placed on ground 
	UPROPERTY(EditAnywhere, Category = "Build|Grid")
	float WallZOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Grid")
	float WallSnapInset = 0.f;

	// Tiny tuning if your pivots aren’t perfect
	UPROPERTY(EditAnywhere, Category = "Build|Door")
	float DoorInsertZOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "Build|Window")
	float WindowInsertZOffset = 0.f;





	// Materials for previewing placement
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build|Preview")
	UMaterialInterface* PreviewValidMat = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build|Preview")
	UMaterialInterface* PreviewInvalidMat = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build|UI")
	TSubclassOf<UBuildingMenu> BuildMenuClass;

	// Current mesh used for preview + placement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	UStaticMesh* CurrentBuildMesh = nullptr;

	// current label ("Build Foundation", "Build Wall")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build")
	FText CurrentBuildName;

	// Open the building menu 
	UFUNCTION(BlueprintCallable, Category = "Build")
	void OpenBuildMenu();

	// Change which mesh we’re previewing/placing 
	UFUNCTION(BlueprintCallable, Category = "Build")
	void SetCurrentBuild(UStaticMesh* NewMesh, const FText& Label, int32 WoodCost, int32 StoneCost);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void StartBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Build")
	bool IsBuildModeActive() const { return IsBuildModeOn; }


	// Sound played when a structure is successfully placed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build|Sound")
	USoundBase* PlaceBuildSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build|Sound")
	USoundBase* OpenBuildMenuSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build|Sound")
	USoundBase* CloseBuildMenuSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build|Sound")
	USoundBase* DestroysBuildSound = nullptr;

	// Destory build preview and exit build mode
	UFUNCTION()
	void StartDestroyMode();

	bool IsDestroyModeActive() const { return bDestroyMode; }

	// ?? Called from player when E is pressed in destroy mode
	void DestroyCurrentTarget();

	void UpdateDestroyTrace();

	bool bDestroyMode = false;

	UPROPERTY()
	AActor* DestroyCandidate = nullptr;

	UFUNCTION(BlueprintCallable)
	void ExitBuildAndCloseMenu();



	UPROPERTY(EditDefaultsOnly, Category = "Build|Torch")
	UStaticMesh* TorchMesh;   // the mesh you use for the torch build icon / preview

	UPROPERTY(EditDefaultsOnly, Category = "Build|Torch")
	TSubclassOf<AActor> TorchClass;

	UPROPERTY(EditDefaultsOnly, Category = "Build|Campfire")
	UStaticMesh* CampfireMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Build|Campfire")
	TSubclassOf<AActor> CampfireClass;

	UPROPERTY(EditDefaultsOnly, Category = "Build|Fence")
	UStaticMesh* FenceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Build|Fence")
	float FenceZOffset = 0.f;


	
	// --- Triangle foundation ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* TriangleFoundationMesh = nullptr;

	// (optional, if you have multiple triangle foundation variations)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	TArray<UStaticMesh*> OtherTriangleFoundationMeshes;

	// --- Triangle walls + ceiling (all "triangle edge pieces") ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* TriangleWallMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* TriangleWallLeftMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* TriangleWallRightMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* TriangleCeilingMesh = nullptr;

	// Any extra triangle wall/ceiling variants you add later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	TArray<UStaticMesh*> OtherTriangleEdgeMeshes;

	UPROPERTY(EditAnywhere, Category = "Build")
	float TriangleWallOnCeilingZOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* RoofMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
	TArray<UStaticMesh*> OtherRoofMeshes;

	UPROPERTY(EditAnywhere, Category = "Build")
	float RoofExtraZOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "Build")
	float RoofSideOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* SmallRoofMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UStaticMesh* CeilingMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
	TArray<UStaticMesh*> OtherCeilingMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Door")
	TSubclassOf<ADoor> DoorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build|Door")
	TSubclassOf<ADoor> StoneDoorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build")
	int32 CurrentWoodCost = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build")
	int32 CurrentStoneCost = 0;


	// --- Helpers ---
	bool IsTriangleFoundationMesh(UStaticMesh* Mesh) const;
	bool IsTriangleEdgeMesh(UStaticMesh* Mesh) const; // walls + ceiling
	bool IsRoofMesh(UStaticMesh* Mesh) const;
	bool IsFoundationMesh(UStaticMesh* Mesh) const;
	bool IsCeilingMesh(UStaticMesh* Mesh) const;
	bool CanAffordCurrentBuild() const;
	void SpendResourcesForCurrentBuild();
	bool IsDoorMesh(UStaticMesh* Mesh) const;
	bool IsDoorFrameMesh(UStaticMesh* Mesh) const;
	bool IsWindowMesh(UStaticMesh* Mesh) const;
	bool IsWindowFrameMesh(UStaticMesh* Mesh) const;
};
