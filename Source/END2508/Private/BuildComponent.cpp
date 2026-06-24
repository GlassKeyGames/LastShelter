// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildComponent.h"
#include "Code/Actors/BaseCharacter.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Both/PlayerHUD.h"
#include "Code/Actors/BasePlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Code/Actors/Door.h"
#include "Code/Widgets/BuildingMenu.h"

// Sets default values for this component's properties
UBuildComponent::UBuildComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UBuildComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<ABaseCharacter>(GetOwner());
	if (!PlayerRef) return;

	// Only players should have build mode
	if (!PlayerRef->IsPlayerControlled())
		return;

	Camera = PlayerRef->FindComponentByClass<UCameraComponent>();

	if (FoundationMesh)
	{
		const FBoxSphereBounds Bounds = FoundationMesh->GetBounds();
		TileSize = Bounds.BoxExtent.X * 2.f; // full width
		UE_LOG(LogTemp, Warning, TEXT("Auto TileSize = %.2f  (Foundation Extent: %s)"),
			TileSize,
			*Bounds.BoxExtent.ToString());
	}

	if (WallMesh && WallZOffset <= 0.f)
	{
		const FBoxSphereBounds WallBounds = WallMesh->GetBounds();
		WallZOffset = WallBounds.BoxExtent.Z; // half height
		UE_LOG(LogTemp, Warning, TEXT("Auto WallZOffset = %.2f  (Wall Extent: %s)"),
			WallZOffset,
			*WallBounds.BoxExtent.ToString());
	}

	if (FenceMesh && FenceZOffset <= 0.f)
	{
		const FBoxSphereBounds FenceBounds = FenceMesh->GetBounds();
		FenceZOffset = FenceBounds.BoxExtent.Z; // half height
		UE_LOG(LogTemp, Warning, TEXT("Auto FenceZOffset = %.2f  (Fence Extent: %s)"),
			FenceZOffset,
			*FenceBounds.BoxExtent.ToString());
	}

	if (!CurrentBuildMesh && FoundationMesh)
	{
		SetCurrentBuild(
			FoundationMesh,
			FText::FromString("Build Foundation"),
			0,
			0
		);
	}
	
}

void UBuildComponent::ToggleBuildMode()
{

	if(!IsBuildModeOn)
	{
		PlayerRef->GetWorldTimerManager().SetTimer(BuildCycleHandle, this, &UBuildComponent::UpdateBuildPreview, 0.1f, true);
		IsBuildModeOn = true;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(BuildCycleHandle);
		if (BuildPreview)
		{
			BuildPreview->DestroyComponent();
			BuildPreview = nullptr;
		}
		DoOnce = true;
		IsBuildModeOn = false;

		// CLOSE SOUND 
		if (CloseBuildMenuSound)
		{
			UGameplayStatics::PlaySound2D(this, CloseBuildMenuSound);
		}
	}
	
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (UPlayerHUD* HUD = BP->GetHUDWidget())
		{
			const FText LabelToUse =
				CurrentBuildName.IsEmpty()
				? FText::FromString(TEXT("Build"))
				: CurrentBuildName;

			HUD->SetInteractTextVisible(
				IsBuildModeOn,
				IsBuildModeOn ? LabelToUse : FText::GetEmpty());
		}
	}
}

void UBuildComponent::PlaceBuildable()
{
	// Only place if we're in build mode, have a player, and the spot is valid
	if (!IsBuildModeOn || !PlayerRef || !CanPlace)
		return;

	if (!CanAffordCurrentBuild())
	{
		if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
		{
			if (UPlayerHUD* HUD = BP->GetHUDWidget())
			{
				HUD->SetInteractTextVisible(true, FText::FromString(TEXT("Not enough resources")));
			}
		}
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
		return;

	UStaticMesh* MeshToUse = CurrentBuildMesh ? CurrentBuildMesh : FoundationMesh;
	if (!MeshToUse)
		return;

	AActor* NewActor = nullptr;

	// ----- TORCH -----
	if (MeshToUse == TorchMesh && TorchClass)
	{
		NewActor = World->SpawnActor<AActor>(TorchClass, BuildTransform);
		if (!NewActor) return;

		NewActor->Tags.Add("Build_Structure");
		NewActor->Tags.Add("Build_Torch");

		SpendResourcesForCurrentBuild();

		if (PlaceBuildSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PlaceBuildSound, BuildTransform.GetLocation());
		}

		return;
	}

	// ----- CAMPFIRE -----
	if (MeshToUse == CampfireMesh && CampfireClass)
	{
		NewActor = World->SpawnActor<AActor>(CampfireClass, BuildTransform);
		if (!NewActor) return;

		NewActor->Tags.Add("Build_Structure");
		NewActor->Tags.Add("Build_Campfire");

		SpendResourcesForCurrentBuild();

		if (PlaceBuildSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PlaceBuildSound, BuildTransform.GetLocation());
		}

		return;
	}

	// ----- DOOR -----
	if (MeshToUse == DoorMesh && DoorClass)
	{
		ADoor* NewDoor = World->SpawnActorDeferred<ADoor>(
			DoorClass,
			BuildTransform,
			GetOwner(),
			Cast<APawn>(GetOwner()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (!NewDoor)
		{
			UE_LOG(LogTemp, Error, TEXT("DOOR SPAWN FAILED"));
			return;
		}

		NewDoor->Tags.Add("Build_Structure");
		NewDoor->Tags.Add("Build_Door");

		NewDoor->FinishSpawning(BuildTransform);

		SpendResourcesForCurrentBuild();

		if (PlaceBuildSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PlaceBuildSound, BuildTransform.GetLocation());
		}

		return;
	}

	// ----- DEFAULT PATH FOR EVERYTHING ELSE -----
	NewActor = World->SpawnActor<AActor>(AActor::StaticClass(), BuildTransform);
	if (!NewActor)
		return;

	if (IsTriangleFoundationMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Foundation");
		NewActor->Tags.Add("Build_Foundation_Triangle");
	}
	else if (IsFoundationMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Foundation");
		NewActor->Tags.Add("Build_Foundation_Square");
	}
	else if (IsCeilingMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Wall");
		NewActor->Tags.Add("Build_Wall_Square");
		NewActor->Tags.Add("Build_Ceiling");
	}
	else if (IsWallMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Wall");
		NewActor->Tags.Add("Build_Wall_Square");
	}
	else if (IsTriangleEdgeMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Wall");
		NewActor->Tags.Add("Build_Wall_Triangle");
	}
	else if (IsRoofMesh(MeshToUse))
	{
		NewActor->Tags.Add("Build_Wall");
		NewActor->Tags.Add("Build_Roof");
	}

	// generic tag for anything placeable
	NewActor->Tags.Add("Build_Structure");

	if (MeshToUse == DoorMesh)
	{
		NewActor->Tags.Add("Build_Door");
	}

	UStaticMeshComponent* SM = NewObject<UStaticMeshComponent>(NewActor);
	SM->RegisterComponent();
	SM->SetStaticMesh(MeshToUse);
	SM->SetMobility(EComponentMobility::Movable);
	SM->SetWorldTransform(BuildTransform);
	SM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SM->SetCollisionResponseToAllChannels(ECR_Block);
	SM->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SM->SetGenerateOverlapEvents(false);
	SM->SetSimulatePhysics(false);

	NewActor->SetRootComponent(SM);

	SpendResourcesForCurrentBuild();

	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		const bool bFoundationPlaced = IsFoundationMesh(MeshToUse);
		const bool bWallPlaced = IsWallMesh(MeshToUse);
		const bool bDoorFramePlaced = (MeshToUse == DoorFrameMesh);
		const bool bCeilingPlaced = IsCeilingMesh(MeshToUse);

		BP->RegisterPlacedBuildByType(
			bFoundationPlaced,
			bWallPlaced,
			bDoorFramePlaced,
			bCeilingPlaced
		);
	}

	// PLAY PLACE SOUND
	if (PlaceBuildSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PlaceBuildSound,
			BuildTransform.GetLocation()
		);
	}

	UE_LOG(LogTemp, Warning, TEXT("Placed build piece: %s"), *MeshToUse->GetName());
}
bool UBuildComponent::IsWallMesh(UStaticMesh* Mesh) const
{
	if (!Mesh)
	{
		return false;
	}

	// The main wall mesh
	if (Mesh == WallMesh)
	{
		return true;
	}

	// Any extra meshes you add in the editor
	return OtherWallMeshes.Contains(Mesh);
}

void UBuildComponent::SetCurrentBuild(UStaticMesh* NewMesh, const FText& Label, int32 WoodCost, int32 StoneCost)
{
	if (!NewMesh)
		return;

	CurrentBuildMesh = NewMesh;
	CurrentBuildName = Label;

	// Force overwrite every time
	CurrentWoodCost = FMath::Max(0, WoodCost);
	CurrentStoneCost = FMath::Max(0, StoneCost);

	UE_LOG(LogTemp, Warning, TEXT("SetCurrentBuild -> %s | Wood=%d | Stone=%d"),
		*Label.ToString(), CurrentWoodCost, CurrentStoneCost);

	if (BuildPreview && CurrentBuildMesh)
	{
		BuildPreview->SetStaticMesh(CurrentBuildMesh);
	}
}

void UBuildComponent::StartBuildMode()
{
	if (!PlayerRef || IsBuildModeOn)
		return;

	// Start the preview timer
	PlayerRef->GetWorldTimerManager().SetTimer(
		BuildCycleHandle,
		this,
		&UBuildComponent::UpdateBuildPreview,
		0.1f,
		true
	);

	IsBuildModeOn = true;

	// Spawn preview immediately on first tick
	DoOnce = true;

	// Update HUD text
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (UPlayerHUD* HUD = BP->GetHUDWidget())
		{
			const FText LabelToUse =
				CurrentBuildName.IsEmpty()
				? FText::FromString(TEXT("Build"))
				: CurrentBuildName;

			HUD->SetInteractTextVisible(true, LabelToUse);
		}
	}
}

void UBuildComponent::OpenBuildMenu()
{
	if (!BuildMenuClass)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	// Close existing menu
	if (ActiveMenu)
	{
		ActiveMenu->RemoveFromParent();
		ActiveMenu = nullptr;
	}

	ActiveMenu = CreateWidget<UBuildingMenu>(World, BuildMenuClass);
	if (!ActiveMenu)
		return;

	// OPEN MENU SOUND
	if (OpenBuildMenuSound)
	{
		UGameplayStatics::PlaySound2D(this, OpenBuildMenuSound);
	}

	ActiveMenu->InitializeMenu(this);
	ActiveMenu->AddToViewport();

	// game + UI focus with mouse
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (APlayerController* PC = Cast<APlayerController>(BP->GetController()))
		{
			FInputModeGameAndUI Mode;
			Mode.SetHideCursorDuringCapture(false);
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(Mode);
			PC->bShowMouseCursor = true;
		}
	}
}

static bool PlacedFrame(AActor* Actor, UStaticMesh* FrameMesh)
{
	if (!Actor || !FrameMesh) return false;

	if (UStaticMeshComponent* SM = Actor->FindComponentByClass<UStaticMeshComponent>())
	{
		return SM->GetStaticMesh() == FrameMesh;
	}

	return false;
}


void UBuildComponent::BuildCycle()
{
	if (!Camera) return;

	FVector StartLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * 350.0f;
	FVector EndLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * 1000.0f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		Params
	);


	// cannot place unless we find a valid spot
	CanPlace = false;

	// --- What are we currently placing? ---
	const bool bIsFence = (CurrentBuildMesh == FenceMesh);
	const bool bPlacingSquareWall = IsWallMesh(CurrentBuildMesh);

	// Triangle sub-types
	const bool bPlacingTriangleWall =
		(CurrentBuildMesh == TriangleWallMesh ||
			CurrentBuildMesh == TriangleWallLeftMesh ||
			CurrentBuildMesh == TriangleWallRightMesh);

	const bool bPlacingTriangleCeiling = (CurrentBuildMesh == TriangleCeilingMesh);
	const bool bPlacingTriangleRoof = IsRoofMesh(CurrentBuildMesh);

	// "Any triangle edge-type" if you still need that concept
	const bool bPlacingTriangleEdge = bPlacingTriangleWall || bPlacingTriangleCeiling || bPlacingTriangleRoof;

	const bool bPlacingAnyWall = bPlacingSquareWall || bPlacingTriangleEdge;
	const bool bPlacingDoor = (CurrentBuildMesh == DoorMesh);
	const bool bPlacingWindow = (CurrentBuildMesh == WindowMesh);
	const bool bPlacingTriangleFoundation = IsTriangleFoundationMesh(CurrentBuildMesh);
	const bool bPlacingSquareFoundation = IsFoundationMesh(CurrentBuildMesh);

	// ======================
	// FENCE: ground only
	// ======================
	if (bIsFence)
	{
		if (bHit)
		{
			FVector P = HitResult.ImpactPoint;

			const float ExtraLift = 2.f;
			P.Z = HitResult.ImpactPoint.Z + FenceZOffset + ExtraLift;

			BuildTransform.SetLocation(P);
			BuildTransform.SetRotation(FQuat::Identity);
			CanPlace = true;

			if (FenceMesh)
			{
				const FBoxSphereBounds FenceBounds = FenceMesh->GetBounds();

				DrawDebugBox(
					GetWorld(),
					P,
					FenceBounds.BoxExtent,
					FQuat::Identity,
					FColor::Green,
					false, 0.f, 0, 2.f
				);
			}

			UE_LOG(LogTemp, Warning,
				TEXT("[BuildComponent] Fence placement at %s (Impact=%s, ZOffset=%.2f)"),
				*P.ToString(),
				*HitResult.ImpactPoint.ToString(),
				FenceZOffset);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[BuildComponent] Fence: no hit, CanPlace=false"));
		}

		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
		return;
	}

	// =========================
	// SNAP TO EXISTING PIECES
	// =========================
	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

		const bool bHitFoundation = HitActor->ActorHasTag("Build_Foundation");
		const bool bHitSquareFoundation = HitActor->ActorHasTag("Build_Foundation_Square");
		const bool bHitTriangleFoundation = HitActor->ActorHasTag("Build_Foundation_Triangle");
		const bool bHitWall = HitActor->ActorHasTag("Build_Wall");
		const bool bHitSquareWall = HitActor->ActorHasTag("Build_Wall_Square");
		const bool bHitTriangleWall = HitActor->ActorHasTag("Build_Wall_Triangle");

		// ---------- INSERT INTO FRAME ----------
		if (bPlacingDoor || bPlacingWindow)
		{
			const bool bHitDoorFrame = PlacedFrame(HitActor, DoorFrameMesh);
			const bool bHitWindowFrame = PlacedFrame(HitActor, WindowFrameMesh);

			if ((bPlacingDoor && bHitDoorFrame) || (bPlacingWindow && bHitWindowFrame))
			{
				UStaticMeshComponent* FrameSM = HitActor->FindComponentByClass<UStaticMeshComponent>();
				if (FrameSM && CurrentBuildMesh)
				{
					const FBoxSphereBounds FrameB = FrameSM->Bounds;
					const FBoxSphereBounds InsertB = CurrentBuildMesh->GetBounds();

					FVector Loc = FrameB.Origin;

					const float FrameBottomZ = FrameB.Origin.Z - FrameB.BoxExtent.Z;
					Loc.Z = FrameBottomZ + InsertB.BoxExtent.Z +
						(bPlacingDoor ? DoorInsertZOffset : WindowInsertZOffset);

					BuildTransform.SetLocation(Loc);
					BuildTransform.SetRotation(FrameSM->GetComponentRotation().Quaternion());

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}
			}

			CanPlace = false;
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
			return;
		}

		// ---------- WALL ? WALL (stack vertically) ----------
		if (bHitWall && bPlacingAnyWall)
		{
			UStaticMeshComponent* HitMesh =
				HitActor->FindComponentByClass<UStaticMeshComponent>();

			if (!HitMesh)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit wall has no StaticMeshComponent, abort stack snap"));
			}
			else
			{
				UStaticMesh* HitStaticMesh = HitMesh->GetStaticMesh();

				const bool bHitIsSquareWall = HitActor->ActorHasTag("Build_Wall_Square");
				const bool bHitIsTriWall = HitActor->ActorHasTag("Build_Wall_Triangle");
				const bool bHitIsRoof = HitActor->ActorHasTag("Build_Roof");
				const bool bHitIsTriangleCeiling = (HitStaticMesh == TriangleCeilingMesh);
				const bool bHitIsRegularCeiling = (HitStaticMesh == CeilingMesh);

				// Shared data for all cases
				const FBoxSphereBounds HitBounds = HitMesh->Bounds;
				const FVector          HitCenter = HitBounds.Origin;
				const FRotator         HitRot = HitMesh->GetComponentRotation();

				// ================================
				// 1) SQUARE WALL ON SQUARE WALL
				// ================================
				if (bPlacingSquareWall && bHitIsSquareWall)
				{
					const float VerticalOffset = HitBounds.BoxExtent.Z * 2.f;

					FVector NewLoc = HitCenter;
					NewLoc.Z += VerticalOffset;

					BuildTransform.SetLocation(NewLoc);
					BuildTransform.SetRotation(HitRot.Quaternion());

					UE_LOG(LogTemp, Warning,
						TEXT("SQUARE WALL STACK: Base=%s NewLoc=%s Extents=%s Rot=%s"),
						*HitCenter.ToString(),
						*NewLoc.ToString(),
						*HitBounds.BoxExtent.ToString(),
						*HitRot.ToString());

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// ==============================================
				// 2) TRIANGLE L/R WALL ON TRIANGLE CEILING/WALL
				//    (pure vertical stack)
				// ==============================================
				if (bPlacingTriangleWall && (bHitIsTriangleCeiling || bHitIsTriWall))
				{
					const float VerticalOffset = HitBounds.BoxExtent.Z * 2.f;

					FVector NewLoc = HitCenter;
					NewLoc.Z += VerticalOffset;

					BuildTransform.SetLocation(NewLoc);
					BuildTransform.SetRotation(HitRot.Quaternion());

					UE_LOG(LogTemp, Warning,
						TEXT("TRI WALL STACK: Base=%s NewLoc=%s Extents=%s Rot=%s"),
						*HitCenter.ToString(),
						*NewLoc.ToString(),
						*HitBounds.BoxExtent.ToString(),
						*HitRot.ToString());

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// ====================================================
				// 3) TRIANGLE L/R WALL ON REGULAR CEILING (top edge)
				// ====================================================
				if (bPlacingTriangleWall && bHitIsRegularCeiling && CurrentBuildMesh)
				{
					const FBoxSphereBounds TriBounds = CurrentBuildMesh->GetBounds();

					const FVector Delta = HitResult.ImpactPoint - HitCenter;

					FVector Dir = FVector::ZeroVector;
					float   Yaw = 0.f;
					float   EdgeHalf = 0.f;        // half size of ceiling along the chosen axis
					float   WallHalfDepth = 0.f;   // how far the flat edge of the triangle extends inward

					if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
					{
						// front/back edges (±X)
						Dir = FVector((Delta.X >= 0.f) ? 1.f : -1.f, 0.f, 0.f);
						EdgeHalf = HitBounds.BoxExtent.X;
						WallHalfDepth = TriBounds.BoxExtent.Y;   // assumes depth in Y
						Yaw = (Dir.X > 0.f) ? -90.f : 90.f;
					}
					else
					{
						// left/right edges (±Y)
						Dir = FVector(0.f, (Delta.Y >= 0.f) ? 1.f : -1.f, 0.f);
						EdgeHalf = HitBounds.BoxExtent.Y;
						WallHalfDepth = TriBounds.BoxExtent.X;   // assumes depth in X
						Yaw = (Dir.Y > 0.f) ? 0.f : 180.f;
					}

					const float Offset = EdgeHalf - WallHalfDepth;

					FVector NewLoc = HitCenter + Dir * Offset;
					// put bottom of triangle wall on top face of the ceiling
					NewLoc.Z = HitCenter.Z + HitBounds.BoxExtent.Z + TriBounds.BoxExtent.Z;

					BuildTransform.SetLocation(NewLoc);
					BuildTransform.SetRotation(FRotator(0.f, Yaw, 0.f).Quaternion());

					UE_LOG(LogTemp, Warning,
						TEXT("TRI WALL ON REG CEILING: Center=%s NewLoc=%s EdgeHalf=%.2f WallDepth=%.2f Yaw=%.1f"),
						*HitCenter.ToString(),
						*NewLoc.ToString(),
						EdgeHalf,
						WallHalfDepth,
						Yaw);

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// ============================================
				// 4) TRIANGLE ROOF ON TRIANGLE L/R WALLS
				//    (top + sideways offset into the gap)
				// ============================================
				if (bPlacingTriangleRoof &&
					(HitStaticMesh == TriangleWallLeftMesh || HitStaticMesh == TriangleWallRightMesh) &&
					CurrentBuildMesh)
				{
					const FBoxSphereBounds RoofBounds = CurrentBuildMesh->GetBounds();

					const float WallHalfHeight = HitBounds.BoxExtent.Z;
					const float RoofHalfHeight = RoofBounds.BoxExtent.Z;

					// Start at wall centre
					FVector RoofLoc = HitCenter;

					// Bottom of roof sits on top of the wall + tweak
					RoofLoc.Z = HitCenter.Z + WallHalfHeight + RoofHalfHeight + RoofExtraZOffset;

					// Slide sideways (between the two triangles)
					const FVector RightDir = HitMesh->GetRightVector();
					const float SideSign = (HitStaticMesh == TriangleWallRightMesh) ? -1.f : 1.f;

					RoofLoc += RightDir * (RoofSideOffset * SideSign);

					BuildTransform.SetLocation(RoofLoc);
					BuildTransform.SetRotation(HitRot.Quaternion());

					UE_LOG(LogTemp, Warning,
						TEXT("TRI ROOF SNAP: WallCenter=%s RoofLoc=%s WallHalf=%.2f RoofHalf=%.2f SideOffset=%.2f ExtraZ=%.2f"),
						*HitCenter.ToString(),
						*RoofLoc.ToString(),
						WallHalfHeight,
						RoofHalfHeight,
						RoofSideOffset,
						RoofExtraZOffset);

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// Any other wall/wall combo falls through to foundation / grid logic
			}
		}

		// ---------- FOUNDATION snapping ----------
		if (!bHitFoundation)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Hit actor is NOT a foundation, using grid fallback"));
		}
		else
		{
			UStaticMeshComponent* HitMesh =
				HitActor->FindComponentByClass<UStaticMeshComponent>();

			if (!HitMesh)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit foundation has no StaticMeshComponent, abort snap"));
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
				return;
			}

			const FBoxSphereBounds HitBounds = HitMesh->Bounds;
			const FVector BaseLoc = HitBounds.Origin;

			// Debug draw
			DrawDebugBox(
				GetWorld(),
				BaseLoc,
				HitBounds.BoxExtent,
				FColor::Yellow,
				false, 0.f, 0, 2.f);

			UE_LOG(LogTemp, Warning,
				TEXT("SNAP BASE: BaseLoc=%s  Impact=%s  Normal=%s  FoundationExtents=%s  PlacingWall=%d"),
				*BaseLoc.ToString(),
				*HitResult.ImpactPoint.ToString(),
				*HitResult.ImpactNormal.ToString(),
				*HitBounds.BoxExtent.ToString(),
				bPlacingAnyWall ? 1 : 0);

			// ---------- FOUNDATION ? FOUNDATION tiling ----------
			if (!bPlacingAnyWall)
			{
				FVector Offset = FVector::ZeroVector;

				if (FMath::Abs(HitResult.ImpactNormal.X) > FMath::Abs(HitResult.ImpactNormal.Y))
				{
					Offset.X = (HitResult.ImpactNormal.X > 0.f)
						? HitBounds.BoxExtent.X * 2.f
						: -HitBounds.BoxExtent.X * 2.f;
				}
				else
				{
					Offset.Y = (HitResult.ImpactNormal.Y > 0.f)
						? HitBounds.BoxExtent.Y * 2.f
						: -HitBounds.BoxExtent.Y * 2.f;
				}

				FVector Snapped = BaseLoc + Offset;
				Snapped.Z = BaseLoc.Z;

				UE_LOG(LogTemp, Warning,
					TEXT("FOUNDATION SNAP: Offset=%s  SnappedLoc=%s"),
					*Offset.ToString(),
					*Snapped.ToString());

				BuildTransform.SetLocation(Snapped);
				BuildTransform.SetRotation(FQuat::Identity);

				CanPlace = true;
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
				return;
			}
			// ---------- FOUNDATION ? WALL snapping ----------
			else
			{
				// SQUARE WALLS ? SQUARE FOUNDATIONS
				if (bPlacingSquareWall)
				{
					if (!bHitSquareFoundation)
					{
						CanPlace = false;
						DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
						return;
					}

					if (!CurrentBuildMesh)
					{
						CanPlace = false;
						DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
						return;
					}

					UStaticMesh* WallMeshToUse = CurrentBuildMesh;
					const FBoxSphereBounds WallBounds = WallMeshToUse->GetBounds();

					const FVector Delta = HitResult.ImpactPoint - BaseLoc;
					const float WallThickness = FMath::Min(WallBounds.BoxExtent.X, WallBounds.BoxExtent.Y);

					FVector Dir = FVector::ZeroVector;
					float Offset = 0.f;
					float Yaw = 0.f;

					if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
					{
						Dir = FVector((Delta.X >= 0.f) ? 1.f : -1.f, 0.f, 0.f);

						const float FoundationHalf = HitBounds.BoxExtent.X;
						Offset = FoundationHalf + WallThickness;

						Yaw = (Dir.X > 0.f) ? -90.f : 90.f;
					}
					else
					{
						Dir = FVector(0.f, (Delta.Y >= 0.f) ? 1.f : -1.f, 0.f);

						const float FoundationHalf = HitBounds.BoxExtent.Y;
						Offset = FoundationHalf + WallThickness;

						Yaw = (Dir.Y > 0.f) ? 0.f : 180.f;
					}

					Offset = FMath::Max(0.f, Offset - WallSnapInset);

					FVector EdgeLoc = BaseLoc + Dir * Offset;
					EdgeLoc.Z = BaseLoc.Z + HitBounds.BoxExtent.Z + WallZOffset;

					UE_LOG(LogTemp, Warning, TEXT("FINAL SQUARE WALL SNAP: Loc=%s  Dir=%s  Offset=%.2f  Yaw=%.1f"),
						*EdgeLoc.ToString(),
						*Dir.ToString(),
						Offset,
						Yaw);

					BuildTransform.SetLocation(EdgeLoc);
					BuildTransform.SetRotation(FRotator(0.f, Yaw, 0.f).Quaternion());

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// TRIANGLE CEILING ? TRIANGLE FOUNDATIONS
				if (bPlacingTriangleCeiling)
				{
					if (!bHitTriangleFoundation)
					{
						CanPlace = false;
						DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
						return;
					}

					if (!CurrentBuildMesh)
					{
						CanPlace = false;
						DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
						return;
					}

					UStaticMesh* CeilingMeshToUse = CurrentBuildMesh;
					const FBoxSphereBounds CeilingBounds = CeilingMeshToUse->GetBounds();

					const FVector Delta = HitResult.ImpactPoint - BaseLoc;
					const float EdgeThickness = FMath::Min(CeilingBounds.BoxExtent.X, CeilingBounds.BoxExtent.Y);

					FVector Dir = FVector::ZeroVector;
					float Offset = 0.f;
					float Yaw = 0.f;

					if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
					{
						Dir = FVector((Delta.X >= 0.f) ? 1.f : -1.f, 0.f, 0.f);
						const float FoundationHalf = HitBounds.BoxExtent.X;
						Offset = FoundationHalf + EdgeThickness;
						Yaw = (Dir.X > 0.f) ? -90.f : 90.f;
					}
					else
					{
						Dir = FVector(0.f, (Delta.Y >= 0.f) ? 1.f : -1.f, 0.f);
						const float FoundationHalf = HitBounds.BoxExtent.Y;
						Offset = FoundationHalf + EdgeThickness;
						Yaw = (Dir.Y > 0.f) ? 0.f : 180.f;
					}

					Offset = FMath::Max(0.f, Offset - WallSnapInset);

					FVector EdgeLoc = BaseLoc + Dir * Offset;
					// Height: top of foundation
					EdgeLoc.Z = BaseLoc.Z + HitBounds.BoxExtent.Z;

					UE_LOG(LogTemp, Warning, TEXT("TRI CEILING SNAP ON TRI FOUNDATION: Loc=%s  Dir=%s  Offset=%.2f  Yaw=%.1f"),
						*EdgeLoc.ToString(),
						*Dir.ToString(),
						Offset,
						Yaw);

					BuildTransform.SetLocation(EdgeLoc);
					BuildTransform.SetRotation(FRotator(0.f, Yaw, 0.f).Quaternion());

					CanPlace = true;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}

				// L/R triangle walls and roofs DO NOT snap directly to foundations
				if (bPlacingTriangleWall || bPlacingTriangleRoof)
				{
					CanPlace = false;
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
					return;
				}
			}
		}
	}

	// ======================
	// FALLBACK: grid snap
	// ======================
	if (bHit)
	{
		const bool bIsWall = IsWallMesh(CurrentBuildMesh); // only square walls treated as ground walls here

		FVector P = HitResult.ImpactPoint;

		if (bIsFence)
		{
			const float ExtraLift = 2.f;
			P.Z = HitResult.ImpactPoint.Z + FenceZOffset + ExtraLift;
		}
		else if (bIsWall)
		{
			P.Z = HitResult.ImpactPoint.Z + WallZOffset;
		}
		else
		{
			const float Half = TileSize * 0.5f;
			P.X = FMath::GridSnap(P.X - Half, TileSize) + Half;
			P.Y = FMath::GridSnap(P.Y - Half, TileSize) + Half;
			P.Z = HitResult.ImpactPoint.Z + 2.f;
		}

		BuildTransform.SetLocation(P);
		BuildTransform.SetRotation(FQuat::Identity);
		CanPlace = true;
	}

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 2.f);
}

void UBuildComponent::SpawnBuildable()
{
	UE_LOG(LogTemp, Warning, TEXT("[BuildComponent] SpawnBuildable called"));

	BuildPreview = Cast<UStaticMeshComponent>(
		PlayerRef->AddComponentByClass(
			UStaticMeshComponent::StaticClass(),
			false,
			BuildTransform,
			false));

	if (!BuildPreview)
	{
		UE_LOG(LogTemp, Error, TEXT("[BuildComponent] FAILED to create BuildPreview component!"));
		return;
	}

	// use current build, fallback to foundation
	UStaticMesh* LoadedMesh = CurrentBuildMesh ? CurrentBuildMesh : FoundationMesh;

	if (LoadedMesh)
	{
		BuildPreview->SetStaticMesh(LoadedMesh);
		BuildPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		BuildPreview->SetVisibility(true, true);
		BuildPreview->SetHiddenInGame(false);
		BuildPreview->SetCastShadow(false);

		UE_LOG(LogTemp, Warning,
			TEXT("[BuildComponent] BuildPreview mesh set to: %s"),
			*LoadedMesh->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[BuildComponent] No mesh available for preview!"));
	}
}

void UBuildComponent::UpdateBuildPreview()
{
	BuildCycle();

	if(DoOnce)
	{
		SpawnBuildable();
		DoOnce = false;
	}


	if (BuildPreview)
	{
		BuildPreview->SetWorldLocation(BuildTransform.GetLocation());
		BuildPreview->SetWorldRotation(BuildTransform.GetRotation().Rotator());

		if (CanPlace && PreviewValidMat)
		{
			BuildPreview->SetMaterial(0, PreviewValidMat);
		}
		else if (!CanPlace && PreviewInvalidMat)
		{
			BuildPreview->SetMaterial(0, PreviewInvalidMat);
		}
	}
}

void UBuildComponent::UpdateDestroyTrace()
{
	if (!Camera || !PlayerRef)
		return;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * 1000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerRef);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	AActor* NewCandidate = nullptr;

	if (bHit && Hit.GetActor())
	{
		AActor* HitActor = Hit.GetActor();

		// Only let destroy mode lock onto placed builds
		if (HitActor->ActorHasTag("Build_Structure") ||
			HitActor->ActorHasTag("Build_Foundation") ||
			HitActor->ActorHasTag("Build_Wall"))
		{
			NewCandidate = HitActor;
		}
	}

	DestroyCandidate = NewCandidate;

	// Update HUD text
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (UPlayerHUD* HUD = BP->GetHUDWidget())
		{
			if (DestroyCandidate)
			{
				FString Label = TEXT("Destroy Build");

				if (DestroyCandidate->ActorHasTag("Build_Wall"))
				{
					Label = TEXT("Destroy Wall");
				}
				else if (DestroyCandidate->ActorHasTag("Build_Foundation"))
				{
					Label = TEXT("Destroy Foundation");
				}

				HUD->SetInteractTextVisible(true, FText::FromString(Label));
			}
			else
			{
				HUD->SetInteractTextVisible(false, FText::GetEmpty());
			}
		}
	}

}

void UBuildComponent::ExitBuildAndCloseMenu()
{
	// Turn off build mode if active
	if (IsBuildModeOn)
	{
		ToggleBuildMode(); // already cleans preview + HUD
	}

	// Close the menu
	if (ActiveMenu)
	{
		ActiveMenu->RemoveFromParent();
		ActiveMenu = nullptr;
	}

	// Give control back to the player
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (APlayerController* PC = Cast<APlayerController>(BP->GetController()))
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void UBuildComponent::DestroyCurrentTarget()
{
	if (!bDestroyMode || !DestroyCandidate)
		return;

	AActor* ToDestroy = DestroyCandidate;
	DestroyCandidate = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Destroying build structure: %s"), *ToDestroy->GetName());

	// PLAY DESTROY SOUND
	if (DestroysBuildSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DestroysBuildSound,
			ToDestroy->GetActorLocation()
		);
	}

	ToDestroy->Destroy();

	// Clear HUD text
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (UPlayerHUD* HUD = BP->GetHUDWidget())
		{
			HUD->SetInteractTextVisible(false, FText::GetEmpty());
		}
	}

	// Optional: stay in destroy mode OR exit after one destroy
	bDestroyMode = false;
}

void UBuildComponent::StartDestroyMode()
{
	if (!PlayerRef)
		return;

	// Turn OFF build mode + preview if active
	if (IsBuildModeOn)
	{
		PlayerRef->GetWorldTimerManager().ClearTimer(BuildCycleHandle);
		IsBuildModeOn = false;

		if (BuildPreview)
		{
			BuildPreview->DestroyComponent();
			BuildPreview = nullptr;
		}

		DoOnce = true;
	}

	bDestroyMode = true;
	DestroyCandidate = nullptr;

	// Clear HUD text initially
	if (ABasePlayer* BP = Cast<ABasePlayer>(PlayerRef))
	{
		if (UPlayerHUD* HUD = BP->GetHUDWidget())
		{
			HUD->SetInteractTextVisible(false, FText::GetEmpty());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("BuildComponent: Destroy mode STARTED"));
}



// Called every frame
void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDestroyMode)
	{
		UpdateDestroyTrace();
	}
}

bool UBuildComponent::IsTriangleFoundationMesh(UStaticMesh* Mesh) const
{
	if (!Mesh) return false;

	if (Mesh == TriangleFoundationMesh)
		return true;

	return OtherTriangleFoundationMeshes.Contains(Mesh);
}

bool UBuildComponent::IsTriangleEdgeMesh(UStaticMesh* Mesh) const
{
	if (!Mesh) return false;

	if (Mesh == TriangleWallMesh ||      // ?? add this
		Mesh == TriangleWallLeftMesh ||
		Mesh == TriangleWallRightMesh ||
		Mesh == TriangleCeilingMesh)
	{
		return true;
	}

	return OtherTriangleEdgeMeshes.Contains(Mesh);
}

bool UBuildComponent::IsRoofMesh(UStaticMesh* Mesh) const
{
	if (!Mesh) return false;

	if (Mesh == RoofMesh || Mesh == SmallRoofMesh)
	{
		return true;
	}

	return OtherRoofMeshes.Contains(Mesh);
}

bool UBuildComponent::IsFoundationMesh(UStaticMesh* Mesh) const
{
	if (!Mesh)
	{
		return false;
	}

	if (Mesh == FoundationMesh)
	{
		return true;
	}

	return OtherFoundationMeshes.Contains(Mesh);
}

bool UBuildComponent::IsCeilingMesh(UStaticMesh* Mesh) const
{
	if (!Mesh)
	{
		return false;
	}

	if (Mesh == CeilingMesh)
	{
		return true;
	}

	return OtherCeilingMeshes.Contains(Mesh);
}
bool UBuildComponent::CanAffordCurrentBuild() const
{
	const ABasePlayer* Player = Cast<ABasePlayer>(PlayerRef);
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanAffordCurrentBuild -> No player"));
		return false;
	}

	const bool bEnoughWood = Player->WoodCount >= CurrentWoodCost;
	const bool bEnoughStone = Player->StoneCount >= CurrentStoneCost;

	UE_LOG(LogTemp, Warning,
		TEXT("CanAffordCurrentBuild -> Player Wood=%d Stone=%d | Need Wood=%d Stone=%d | EnoughWood=%d EnoughStone=%d"),
		Player->WoodCount,
		Player->StoneCount,
		CurrentWoodCost,
		CurrentStoneCost,
		bEnoughWood ? 1 : 0,
		bEnoughStone ? 1 : 0
	);

	return bEnoughWood && bEnoughStone;
}

void UBuildComponent::SpendResourcesForCurrentBuild()
{
	ABasePlayer* Player = Cast<ABasePlayer>(PlayerRef);
	if (!Player) return;

	UE_LOG(LogTemp, Warning,
		TEXT("Before Spend -> Wood=%d Stone=%d | Spending Wood=%d Stone=%d"),
		Player->WoodCount,
		Player->StoneCount,
		CurrentWoodCost,
		CurrentStoneCost
	);

	Player->WoodCount = FMath::Max(0, Player->WoodCount - CurrentWoodCost);
	Player->StoneCount = FMath::Max(0, Player->StoneCount - CurrentStoneCost);

	Player->UpdateResourceHUD();

	UE_LOG(LogTemp, Warning,
		TEXT("After Spend -> Wood=%d Stone=%d"),
		Player->WoodCount,
		Player->StoneCount
	);
}