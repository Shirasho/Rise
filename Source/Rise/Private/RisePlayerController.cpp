#include "RisePlayerController.h"

#include "EngineUtils.h"
#include "Landscape.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UObject/SoftObjectPtr.h"

#include "RiseGameMode.h"
#include "RiseHelpers.h"
#include "RiseLog.h"
#include "RisePlayer.h"
#include "RisePlayerState.h"
#include "Components/RiseSelectableComponent.h"
#include "Libraries/RiseActorLibrary.h"
#include "Volumes/RiseCameraBoundsVolume.h"

#define BIND_CAMERA_FUNCTION(Function, Direction, Curve) \
FOnTimelineFloat Camera##Direction##Function##Interp{}; \
FOnTimelineEvent Camera##Direction##Function##Finished{}; \
Camera##Direction##Function##Interp.BindUFunction(this, MAKE_NAME(OnCamera##Direction##Function##Interp)); \
Camera##Direction##Function##Finished.BindUFunction(this, MAKE_NAME(OnCamera##Direction##Function##Finished)); \
if (##Curve) \
{ \
	Camera##Direction##Function##Drift.Timeline.AddInterpFloat(##Curve, Camera##Direction##Function##Interp, FName("Delta")); \
} \
Camera##Direction##Function##Drift.Timeline.SetLooping(false) \

#define BIND_CAMERA_FUNCTION_RAW(Function, Curve) \
FOnTimelineFloat Camera##Function##Interp{}; \
FOnTimelineEvent Camera##Function##Finished{}; \
Camera##Function##Interp.BindUFunction(this, MAKE_NAME(OnCamera##Function##Interp)); \
Camera##Function##Finished.BindUFunction(this, MAKE_NAME(OnCamera##Function##Finished)); \
if (##Curve) \
{ \
	Camera##Function.AddInterpFloat(##Curve, Camera##Function##Interp, FName("Delta")); \
} \
Camera##Function.SetLooping(false) \

#define CAMERA_FUNCTION_IMPL(Function, Direction) \
void ARisePlayerController::OnCamera##Direction##Function##Interp(float TimelineValue) \
{ \
	Camera##Direction##Function##Drift.Drift = TimelineValue; \
} \
void ARisePlayerController::OnCamera##Direction##Function##Finished() { Camera##Direction##Function##Drift.Drift = 0;  }

ARisePlayerController::ARisePlayerController()
{
	CameraHorizontalPanDrift = FRiseCameraDrift();
	CameraVerticalPanDrift = FRiseCameraDrift();
	CameraZoom = FTimeline();

	MinimumCameraSpeed = 5;
	MaximumCameraSpeed = 20;
	MinimumCameraZoom = 400;
	MaximumCameraZoom = 1500;
	CameraZoomStep = 25;
	CameraPanThreshold = 20;
	CameraVerticalAxisValue = 0;
	CameraHorizontalAxisValue = 0;
}

void ARisePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Bind camera functions to input
	BIND_CAMERA_FUNCTION(Pan, Horizontal, CameraPanCurve);
	BIND_CAMERA_FUNCTION(Pan, Vertical, CameraPanCurve);
	BIND_CAMERA_FUNCTION_RAW(Zoom, CameraZoomCurve);

	ARisePlayer* GamePawn = GetRisePlayer();
	check(GamePawn);

	CurrentCameraZoomStep = GamePawn->CameraSpringArmComponent->TargetArmLength;
	TargetCameraZoomStep = CurrentCameraZoomStep;

	for (TActorIterator<ARiseCameraBoundsVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		CameraBoundsVolume = *ActorItr;
		break;
	}

	if (!CameraBoundsVolume)
	{
		UE_LOG(LogRise, Warning, TEXT("No camera bounding volume was found."));
	}
}

void ARisePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	// Allow the mouse to interact with both the game and the UI by default.
	// TODO: When tutorials are implemented and enabled, this will default to FInputModeUIOnly
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	SetInputMode(InputMode);

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// Always show the mouse cursor.
	SetShowMouseCursor(true);

	if (InputComponent)
	{
		InputComponent->BindAction("Select", IE_Pressed, this, &ARisePlayerController::StartSelectionFrame);
		InputComponent->BindAction("Select", IE_Released, this, &ARisePlayerController::EndSelectionFrame);
		InputComponent->BindAction("SelectInverse", IE_Pressed, this, &ARisePlayerController::StartInverseSelectionFrame);
		InputComponent->BindAction("SelectInverse", IE_Released, this, &ARisePlayerController::EndInverseSelectionFrame);

		InputComponent->BindAction(TEXT("ZoomCameraIn"), IE_Pressed, this, &ARisePlayerController::ZoomCameraIn);
		InputComponent->BindAction(TEXT("ZoomCameraOut"), IE_Pressed, this, &ARisePlayerController::ZoomCameraOut);

		InputComponent->BindAxis(TEXT("PanCameraUp"), this, &ARisePlayerController::PanCameraUp);
		InputComponent->BindAxis(TEXT("PanCameraDown"), this, &ARisePlayerController::PanCameraDown);
		InputComponent->BindAxis(TEXT("PanCameraLeft"), this, &ARisePlayerController::PanCameraLeft);
		InputComponent->BindAxis(TEXT("PanCameraRight"), this, &ARisePlayerController::PanCameraRight);
	}
}

void ARisePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	APawn* PlayerPawn = GetPawnOrSpectator();
	if (!PlayerPawn)
	{
		return;
	}

	if (!bCameraMovementDisabled)
	{
		CameraHorizontalPanDrift.Timeline.TickTimeline(DeltaTime);
		CameraVerticalPanDrift.Timeline.TickTimeline(DeltaTime);
		CameraZoom.TickTimeline(DeltaTime);

		// Apply camera pan.
		if (GEngine)
		{
			const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			const float ScrollBorderRight = ViewportSize.X - CameraPanThreshold;
			const float ScrollBorderTop = ViewportSize.Y - CameraPanThreshold;

			float MouseX, MouseY;
			if (GetMousePosition(MouseX, MouseY))
			{
				if (MouseX < CameraPanThreshold)
				{
					CameraHorizontalAxisValue -= 1 - (MouseX / CameraPanThreshold);
				}
				else if (MouseX >= ScrollBorderRight)
				{
					CameraHorizontalAxisValue += (MouseX - ScrollBorderRight) / CameraPanThreshold;
				}

				if (MouseY <= CameraPanThreshold)
				{
					CameraVerticalAxisValue += 1 - (MouseY / CameraPanThreshold);
				}
				else if (MouseY >= ScrollBorderTop)
				{
					CameraVerticalAxisValue -= (MouseY - ScrollBorderTop) / CameraPanThreshold;
				}
			}
		}

		CameraHorizontalAxisValue = FMath::Clamp(CameraHorizontalAxisValue, -1.f, 1.f);
		CameraVerticalAxisValue = FMath::Clamp(CameraVerticalAxisValue, -1.f, 1.f);

		float HorizontalPanSpeed = CalculateCameraMovementSpeed() * CameraHorizontalAxisValue * CameraHorizontalPanDrift.Drift;
		float VerticalPanSpeed = CalculateCameraMovementSpeed() * CameraVerticalAxisValue * CameraVerticalPanDrift.Drift;

		FTransform PlayerTransform = PlayerPawn->GetActorTransform();
		FVector HorizontalPanVector = PlayerTransform.GetRotation().GetRightVector() * CameraHorizontalAxisValue;
		FVector VerticalPanVector = PlayerTransform.GetRotation().GetForwardVector() * CameraVerticalAxisValue;
		FVector NewCameraLocation = PlayerTransform.GetLocation() + HorizontalPanVector + VerticalPanVector;
		
		FocusCameraOnWorldLocation(NewCameraLocation);

		// Get hovered actor.
		// TODO: Is this more efficient to do through built-in collision events? Does such a thing exist?
		AActor* OldHoveredActor = HoveredActor;
		HoveredActor = nullptr;

		TArray<FHitResult> HitResults;
		if (GetHitResultsUnderCursor(HitResults))
		{
			for (FHitResult& HitResult : HitResults)
			{
				HoveredWorldPosition = HitResult.Location;

				AActor* HitActor = HitResult.GetActor();
				if (!HitActor || Cast<ALandscape>(HitActor))
				{
					continue;
				}

				URiseSelectableComponent* SelectableComponent = HitActor->FindComponentByClass<URiseSelectableComponent>();
				if (!SelectableComponent)
				{
					continue;
				}

				HoveredActor = HitActor;
				break;
			}
		}

		if (HoveredActor != OldHoveredActor)
		{
			if (IsValid(OldHoveredActor))
			{
				URiseSelectableComponent* SelectableComponent = OldHoveredActor->FindComponentByClass<URiseSelectableComponent>();
				if (IsValid(SelectableComponent))
				{
					SelectableComponent->UnhoverActor();
				}
			}

			if (IsValid(HoveredActor))
			{
				URiseSelectableComponent* SelectableComponent = HoveredActor->FindComponentByClass<URiseSelectableComponent>();
				if (IsValid(SelectableComponent))
				{
					SelectableComponent->HoverActor();
				}
			}

			NotifyHoveredActorChanged(HoveredActor);
		}

		// Remove dead selected actors.
		int OldSelectedActorsCount = SelectedActors.Num();
		for (int32 SelectedActorIndex = OldSelectedActorsCount - 1; SelectedActorIndex >= 0; --SelectedActorIndex)
		{
			AActor* Actor = SelectedActors[SelectedActorIndex];
			if (!IsValid(Actor))
			{
				SelectedActors.RemoveAt(SelectedActorIndex);
				continue;
			}
		}

		if (SelectedActors.Num() != OldSelectedActorsCount)
		{
			NotifySelectedActorsChanged(SelectedActors);
		}
	}
}

void ARisePlayerController::Surrender(bool bConfirm)
{
	if (bConfirm)
	{
		//TODO: Display UI asking whether the player wants to surrender, only proceeding
		//      if the player confirms.
	}


	if (IsNetMode(NM_Client))
	{
		UE_LOG(LogRise, Log, TEXT("%s has surrendered."), *GetName());
	}

	ServerSurrender();
}

bool ARisePlayerController::ServerSurrender_Validate()
{
	return true;
}

void ARisePlayerController::ServerSurrender_Implementation()
{
	UE_LOG(LogRise, Log, TEXT("%s has surrendered."), *GetName());

	ARiseGameMode* GameMode = Cast<ARiseGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->NotifyPlayerResigned(this);
	}
}

void ARisePlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	ClientGameHasEnded(bIsWinner);
}

void ARisePlayerController::ClientGameHasEnded_Implementation(bool bIsWinner)
{
	NotifyGameHasEnded(bIsWinner);
}

void ARisePlayerController::OnPlayerStateAvailable(ARisePlayerState* NewPlayerState)
{
	if (IsValid(NewPlayerState))
	{
		NewPlayerState->DiscoverOwnedActors();
	}
}

void ARisePlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	if (!IsValid(PlayerState))
	{
		return;
	}

	OnPlayerStateAvailable(GetPlayerState());
}

void ARisePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsValid(PlayerState))
	{
		return;
	}

	OnPlayerStateAvailable(GetPlayerState());
}

ARisePlayer* const ARisePlayerController::GetRisePlayer() const
{
	return Cast<ARisePlayer>(GetPawn());
}

float ARisePlayerController::CalculateCameraMovementSpeed() const
{
	ARisePlayer* GamePawn = GetRisePlayer();
	if (!GamePawn)
	{
		return 0;
	}

	check(GamePawn->CameraSpringArmComponent);

	float SpringArmLength = GamePawn->CameraSpringArmComponent->TargetArmLength;
	return FMath::Clamp(SpringArmLength / 100, MinimumCameraSpeed, MaximumCameraSpeed);
}

void ARisePlayerController::SetCameraMovementEnabled(bool bEnabled)
{
	bCameraMovementDisabled = !bEnabled;
}

bool ARisePlayerController::GetCameraMovementEnabled() const
{
	return bCameraMovementDisabled;
}

AActor* ARisePlayerController::GetHoveredActor() const
{
	return HoveredActor;
}

ARisePlayerState* ARisePlayerController::GetPlayerState() const
{
	return Cast<ARisePlayerState>(PlayerState);
}

TArray<AActor*> ARisePlayerController::GetSelectedActors() const
{
	return SelectedActors;
}

bool ARisePlayerController::GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults) const
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return false;
	}

	FVector2D MousePosition;
	if (!LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
	{
		return false;
	}

	return GetHitResultsUnderScreenPosition(MousePosition, OutHitResults);
}

bool ARisePlayerController::GetHitResultsUnderScreenPosition(const FVector2D& ScreenPosition, TArray<FHitResult>& OutHitResults) const
{
	FVector WorldPosition;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition, WorldPosition, WorldDirection))
	{
		return false;
	}

	return GetHitResultsUnderWorldPositionWithDirection(WorldPosition, WorldDirection, OutHitResults);
}

bool ARisePlayerController::GetHitResultsUnderWorldPosition(const FVector& WorldPosition, TArray<FHitResult>& OutHitResults) const
{
	return GetHitResultsUnderWorldPositionWithDirection(FVector(WorldPosition.X, WorldPosition.Y, HitResultTraceDistance / 2), -FVector::UpVector, OutHitResults);
}

bool ARisePlayerController::GetHitResultsUnderWorldPositionWithDirection(const FVector& WorldPosition, const FVector& WorldDirection, TArray<FHitResult>& OutHitResults) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FCollisionObjectQueryParams Params(FCollisionObjectQueryParams::InitType::AllObjects);

	return World->LineTraceMultiByObjectType(OutHitResults, WorldPosition, WorldPosition + WorldDirection * HitResultTraceDistance, Params);
}

bool ARisePlayerController::GetHitResultsUnderSelectionFrame(TArray<FHitResult>& OutHitResults) const
{
	FIntRect SelectionFrame;
	if (!GetSelectionFrame(SelectionFrame))
	{
		return false;
	}

	// This is too small to be a real select box, so treat it as a left-click.
	if (SelectionFrame.Area() < 10)
	{
		return GetHitResultsUnderCursor(OutHitResults);
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	OutHitResults.Reset();

	for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;
		FVector2D ActorScreenPosition;

		if (UGameplayStatics::ProjectWorldToScreen(this, Actor->GetActorLocation(), ActorScreenPosition) &&
			SelectionFrame.Contains(FIntPoint(ActorScreenPosition.X, ActorScreenPosition.Y)))
		{
			FHitResult HitResult(Actor, nullptr, Actor->GetActorLocation(), FVector());
			OutHitResults.Add(HitResult);
		}
	}

	return OutHitResults.Num() > 0;
}

bool ARisePlayerController::GetSelectionFrame(FIntRect& OutSelectionFrame) const
{
	if (!bCreatingSelectionFrame)
	{
		return false;
	}

	float MouseX, MouseY;
	if (!GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	OutSelectionFrame = FIntRect(
		FIntPoint(
			FMath::Min(SelectionFrameStartPosition.X, MouseX),
			FMath::Max(SelectionFrameStartPosition.X, MouseX)
		),
		FIntPoint(
			FMath::Min(SelectionFrameStartPosition.Y, MouseY),
			FMath::Max(SelectionFrameStartPosition.Y, MouseY)
		)
	);

	return true;
}

bool ARisePlayerController::IsActorSelectable(const AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return false;
	}

	URiseSelectableComponent* SelectableComponent = Actor->FindComponentByClass<URiseSelectableComponent>();
	return !!SelectableComponent;
}

bool ARisePlayerController::DeselectActor(AActor* Actor)
{
	return DeselectActor(Actor, false);
}

bool ARisePlayerController::DeselectActor(AActor* Actor, bool bSuppressNotification)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	if (!SelectedActors.Contains(Actor))
	{
		return false;
	}

	URiseSelectableComponent* SelectableComponent = Actor->FindComponentByClass<URiseSelectableComponent>();
	if (!IsValid(SelectableComponent))
	{
		return false;
	}

	SelectableComponent->DeselectActor();
	SelectedActors.Remove(Actor);

	UE_LOG(LogRise, Log, TEXT("Deselected actor %s"), *Actor->GetName());

	if (!bSuppressNotification)
	{
		NotifySelectedActorsChanged(SelectedActors);
	}

	return true;
}

bool ARisePlayerController::SelectActor(AActor* Actor, bool bAppend)
{
	// I'd rather not allocate a new array here, but this is micro-optimization.
	TArray<AActor*> Actors;
	Actors.Add(Actor);

	return SelectActors(Actors, bAppend) > 0;
}

bool ARisePlayerController::SelectActor(AActor* Actor, bool bAppend, bool bSuppressNotification)
{
	// I'd rather not allocate a new array here, but this is micro-optimization.
	TArray<AActor*> Actors;
	Actors.Add(Actor);

	return SelectActors(Actors, bAppend, bSuppressNotification) > 0;
}

int32 ARisePlayerController::SelectActors(TArray<AActor*> Actors, bool bAppend)
{
	return SelectActors(Actors, bAppend, false);
}

int32 ARisePlayerController::SelectActors(TArray<AActor*> Actors, bool bAppend, bool bSuppressNotification)
{
	int AddedActors = 0;

	if (!bAppend)
	{
		for (AActor* SelectedActor : SelectedActors)
		{
			DeselectActor(SelectedActor);
		}

		SelectedActors.Reset();
	}

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		URiseSelectableComponent* AddedSelectableComponent = Actor->FindComponentByClass<URiseSelectableComponent>();
		if (!IsValid(AddedSelectableComponent))
		{
			continue;
		}

		if (SelectedActors.AddUnique(Actor) == 1)
		{
			AddedSelectableComponent->SelectActor();

			UE_LOG(LogRise, Log, TEXT("Selected actor %s"), *Actor->GetName());

			if (URiseActorLibrary::IsActorOwnedByLocalPlayer(Actor))
			{
				USoundCue* SoundToPlay = AddedSelectableComponent->GetSelectedSound();
				if (IsValid(SoundToPlay))
				{
					//TODO: We may want to throttle how quickly this plays.
					UGameplayStatics::PlaySound2D(this, SoundToPlay);
				}
			}

			++AddedActors;
		}
	}

	if (!bSuppressNotification && AddedActors > 0)
	{
		NotifySelectedActorsChanged(SelectedActors);
	}

	return AddedActors;
}\

void ARisePlayerController::NotifySelectedActorsChanged(const TArray<AActor*>& NewSelectedActors)
{
	OnSelectedActorsChanged(NewSelectedActors);
}

void ARisePlayerController::NotifyActorOwnerChanged(AActor* Actor)
{
	OnActorOwnerChanged(Actor);
}

void ARisePlayerController::NotifyHoveredActorChanged(AActor* Actor)
{
	OnHoveredActorChanged(Actor);
}

void ARisePlayerController::NotifyTeamChanged(ARiseTeamInfo* NewTeam)
{
	OnTeamChanged(NewTeam);
}

void ARisePlayerController::NotifyErrorOccurred(const FString& ErrorMessage)
{
	OnErrorOccurred(ErrorMessage);
}

void ARisePlayerController::NotifyGameHasEnded(bool bIsWinner)
{
	OnGameHasEnded(bIsWinner);
}

ARiseTeamInfo* ARisePlayerController::GetTeamInfo() const
{
	ARisePlayerState* CurrentPlayerState = GetPlayerState();
	if (CurrentPlayerState)
	{
		return CurrentPlayerState->GetTeam();
	}

	return nullptr;
}

void ARisePlayerController::FocusCameraOnWorldLocation2D(const FVector2D& NewCameraLocation)
{
	FocusCameraOnWorldLocation(FVector(NewCameraLocation.X, NewCameraLocation.Y, 0.f));
}

void ARisePlayerController::FocusCameraOnWorldLocation(const FVector& NewCameraLocation)
{
	APawn* PlayerPawn = GetPawnOrSpectator();
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	FVector DesiredCameraLocation = FVector(NewCameraLocation.X, NewCameraLocation.Y, PlayerPawn->GetActorLocation().Z);
	if (IsValid(CameraBoundsVolume) && !CameraBoundsVolume->EncompassesPoint(DesiredCameraLocation))
	{
		FBoxSphereBounds Bounds = CameraBoundsVolume->GetBounds();

		DesiredCameraLocation.X = FMath::Clamp(DesiredCameraLocation.X, Bounds.GetBox().Min.X, Bounds.GetBox().Max.X);
		DesiredCameraLocation.Y = FMath::Clamp(DesiredCameraLocation.Y, Bounds.GetBox().Min.Y, Bounds.GetBox().Max.Y);
	}

	PlayerPawn->SetActorLocation(DesiredCameraLocation);
}

void ARisePlayerController::FocusCameraOnActor(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	FVector ActorLocation = Actor->GetActorLocation();
	FocusCameraOnWorldLocation(ActorLocation);
}

void ARisePlayerController::FocusCameraOnActors(TArray<AActor*> Actors, bool bAllowCameraZoom)
{
	FVector2D TargetLocation = FVector2D::ZeroVector;
	int32 NumActors = 0;

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		FVector ActorLocation = Actor->GetActorLocation();
		TargetLocation.X += ActorLocation.X;
		TargetLocation.Y += ActorLocation.Y;

		++NumActors;
	}

	TargetLocation /= NumActors;

	if (!bAllowCameraZoom || !GEngine)
	{
		FocusCameraOnWorldLocation2D(TargetLocation);
		return;
	}

	// We now have the point that is the average center of all the actors.
	// We should check if the camera is zoomed out enough to see all of the
	// actors, and if not we need to zoom out to the smallest step that makes
	// the units all visible.

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	// We want to add some padding so the selected actors aren't on the very edge of the screen.
	const int32 CameraBufferSpace = 20;

	// Snap the camera zoom to the final location.
	CameraZoom.Stop();
	OnCameraZoomFinished();

	bool bAllActorsVisible;
	do
	{
		bAllActorsVisible = true;
		for (AActor* Actor : Actors)
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			FVector2D ScreenLocation;
			if (!ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenLocation))
			{
				bAllActorsVisible = false;
				break;
			}

			if (ScreenLocation.X < CameraBufferSpace || ScreenLocation.X > ViewportSize.X - CameraBufferSpace ||
				ScreenLocation.Y < CameraBufferSpace || ScreenLocation.Y > ViewportSize.Y - CameraBufferSpace)
			{
				bAllActorsVisible = false;
				break;
			}
		}

		if (!bAllActorsVisible)
		{
			if (TargetCameraZoomStep == MaximumCameraZoom)
			{
				break;
			}

			// Force the camera to zoom out.
			TargetCameraZoomStep = FMath::Min(TargetCameraZoomStep + CameraZoomStep, MaximumCameraZoom);
			OnCameraZoomFinished();
		}

	} while (!bAllActorsVisible);
}

void ARisePlayerController::StartSelectionFrame()
{
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		SelectionFrameStartPosition = FVector2D(MouseX, MouseY);
		bCreatingSelectionFrame = true;
	}
}

void ARisePlayerController::EndSelectionFrame()
{
	if (!bCreatingSelectionFrame)
	{
		return;
	}

	TArray<FHitResult> HitResults;

	if (!GetHitResultsUnderSelectionFrame(HitResults))
	{
		bCreatingSelectionFrame = false;
		return;
	}
	
	bool bHasActorSelectionChanged = false;

	for (FHitResult& HitResult : HitResults)
	{
		AActor* Actor = HitResult.GetActor();
		if (!IsActorSelectable(Actor))
		{
			continue;
		}

		if (bInverseSelectionHotkeyPressed)
		{
			if (SelectedActors.Contains(Actor))
			{
				bHasActorSelectionChanged |= DeselectActor(Actor, true);
			}
			else
			{
				bHasActorSelectionChanged |= SelectActor(Actor, true, true);
			}
		}
		else
		{
			bHasActorSelectionChanged |= SelectActor(Actor, true, true);
		}
	}

	if (bHasActorSelectionChanged)
	{
		NotifySelectedActorsChanged(SelectedActors);
	}

	bCreatingSelectionFrame = false;
}

void ARisePlayerController::StartInverseSelectionFrame()
{
	//TODO: We need to verify this will not interfere with UI.
	bInverseSelectionHotkeyPressed = true;
}

void ARisePlayerController::EndInverseSelectionFrame()
{
	//TODO: We need to verify this will not interfere with UI.
	bInverseSelectionHotkeyPressed = false;
}

CAMERA_FUNCTION_IMPL(Pan, Horizontal)
CAMERA_FUNCTION_IMPL(Pan, Vertical)

void ARisePlayerController::OnCameraZoomInterp(float TimelineValue)
{
	ARisePlayer* GamePawn = GetRisePlayer();
	check(GamePawn);

	GamePawn->CameraSpringArmComponent->TargetArmLength = FMath::Lerp(CurrentCameraZoomStep, TargetCameraZoomStep, TimelineValue);
}

void ARisePlayerController::OnCameraZoomFinished()
{
	ARisePlayer* GamePawn = GetRisePlayer();
	check(GamePawn);

	GamePawn->CameraSpringArmComponent->TargetArmLength = TargetCameraZoomStep;
}

void ARisePlayerController::PanCameraUp(float Scale)
{
	CameraVerticalAxisValue = Scale;

	if (Scale >= .0001f)
	{
		CameraVerticalPanDrift.Timeline.PlayFromStart();
	}
}

void ARisePlayerController::PanCameraDown(float Scale)
{
	CameraVerticalAxisValue = Scale;

	if (Scale >= .0001f)
	{
		CameraVerticalPanDrift.Timeline.PlayFromStart();
	}
}

void ARisePlayerController::PanCameraLeft(float Scale)
{
	CameraHorizontalAxisValue = Scale;

	if (Scale >= .0001f)
	{
		CameraHorizontalPanDrift.Timeline.PlayFromStart();
	}
}

void ARisePlayerController::PanCameraRight(float Scale)
{
	CameraHorizontalAxisValue = Scale;

	if (Scale >= .0001f)
	{
		CameraHorizontalPanDrift.Timeline.PlayFromStart();
	}
}

void ARisePlayerController::ZoomCameraIn()
{
	if (bCameraMovementDisabled)
	{
		return;
	}

	// Start/Restart our camera zoom drift timeline.
	CameraZoom.Stop();

	ARisePlayer* GamePawn = GetRisePlayer();
	check(GamePawn);

	TargetCameraZoomStep = FMath::Clamp(TargetCameraZoomStep - CameraZoomStep, MinimumCameraZoom, MaximumCameraZoom);
	CurrentCameraZoomStep = GamePawn->CameraSpringArmComponent->TargetArmLength;

	CameraZoom.PlayFromStart();
}

void ARisePlayerController::ZoomCameraOut()
{
	if (bCameraMovementDisabled)
	{
		return;
	}

	// Start/Restart our camera zoom drift timeline.
	CameraZoom.Stop();

	ARisePlayer* GamePawn = GetRisePlayer();
	check(GamePawn);

	TargetCameraZoomStep = FMath::Clamp(TargetCameraZoomStep + CameraZoomStep, MinimumCameraZoom, MaximumCameraZoom);
	CurrentCameraZoomStep = GamePawn->CameraSpringArmComponent->TargetArmLength;

	CameraZoom.PlayFromStart();
}

void ARisePlayerController::OnInput_Debug()
{
	//FTransform SpawnActorTransform(FVector::ZeroVector);

	//RiseHelpers::FBlueprintClassFinder<ARiseCommandStation> CommandStationBP(TEXT("/Game/Blueprints/Structures"), TEXT("CommandStation"));
	//if (CommandStationBP.Succeeded())
	//{
	//	UClass* BPClass = CommandStationBP.Class.LoadSynchronous();
	//	ARiseCommandStation* SpawnedActor = GetWorld()->SpawnActorDeferred<ARiseCommandStation>(BPClass, SpawnActorTransform);

	//	//if (!SpawnedActor->TrySetStructureLevel(2))
	//	//{
	//	//	DEBUG_WARNING(TEXT("Unable to set structure to level 2."));
	//	//}

	//	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnActorTransform);
	//}
}