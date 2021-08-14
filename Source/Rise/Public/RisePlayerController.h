#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "RisePlayerController.generated.h"

class ARiseCameraBoundsVolume;
class ARisePlayer;
class ARisePlayerState;
class ARiseTeamInfo;

/**
 * The base PlayerController class for Rise game modes.
 */
UCLASS()
class RISE_API ARisePlayerController : public APlayerController
{
	GENERATED_BODY()

private:

private:

	/** The slowest the camera can move while panning. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (ClampMin = "0", ClampMax = "25"))
	float PanCameraMinimumSpeed;

	/** The fastest the camera can move while panning. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (ClampMin = "0", ClampMax = "25"))
	float PanCameraMaximumSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera")
	bool bPanCameraRegionPercent;

	/**
	 * The number of pixels from the edge of the screen in which to start panning the camera in that direction.
	 * 
	 * @note This value is incompatible with PanCameraRegionPercent.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (DisplayAfter="bPanCameraRegionPercent", EditCondition="!bPanCameraRegionPercent"))
	int32 PanCameraRegionPixels;

	/**
	 * The percent size of the horizontal screen to use to calculate a pan region.
	 * 
	 * Example: 0.05 at 1080p is the equivalent of setting PanCameraRegionPixels to 96.
	 *
	 * @note This value is incompatible with PanCameraRegionPixels.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (DisplayAfter = "PanCameraRegionPixels", EditCondition="bPanCameraRegionPercent"))
	float PanCameraRegionPercent;

	/** The time it takes to ease the camera pan to a full stop on an axis after the camera pan input is released. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (ClampMin = "0"))
	float PanCameraEaseDuration;

	/** The closest the camera can zoom into. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera")
	float ZoomCameraMinimumDistance;

	/** The furthest the camera can zoom out to. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera")
	float ZoomCameraMaximumDistance;

	/** How much to zoom the camera in and out at once. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (ClampMin = "0"))
	float ZoomCameraStep;
	
	/** The time it takes to ease the camera zoom to a full stop on an axis after the camera zoom input is released. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta = (ClampMin = "0"))
	float ZoomCameraEaseDuration;

	/** The camera volume bounds that restrict camera movement for this player. */
	UPROPERTY()
	ARiseCameraBoundsVolume* CameraBoundsVolume;

	/** Whether camera movement is disabled. */
	bool bCameraMovementDisabled;

	/** The actor that is currently being hovered over by this player. */
	UPROPERTY()
	AActor* HoveredActor;

	/** The world coordinates that the cursor is currently hovering over. */
	FVector HoveredWorldPosition;

	/** The actors that are currently selected by this player. */
	UPROPERTY()
	TArray<AActor*> SelectedActors;

	/** Whether this player is in the process of creating an actor selection frame (by dragging the mouse for example). */
	bool bCreatingSelectionFrame;

	/** The screen coordinates that the actor selection frame started on. */
	FVector2D SelectionFrameStartPosition;

	/** Whether the units in the selection frame will be removed or added from the current selection instead of replacing the current selection. */
	bool bInverseSelectionHotkeyPressed;

public:

	ARisePlayerController();

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:

	virtual void PlayerTick(float DeltaTime) override;
	virtual void InitPlayerState() override;
	virtual void OnRep_PlayerState() override;
	virtual void GameHasEnded(AActor* EndGameFocus = NULL, bool bIsWinner = false) override;

	/**
	 * Gets the player state of this player.
	 *
	 * @return The player state of this player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	ARisePlayerState* GetPlayerState() const;

	/**
	 * Gets the team this player belongs to.
	 *
	 * @return The team this player belongs to.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	ARiseTeamInfo* GetTeamInfo() const;

	/**
	 * Gets the actor that is being hovered over (with the mouse for example) by this player.
	 * 
	 * @return The actor that is currently being hovered over by this player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	AActor* GetHoveredActor() const;

	/**
	 * Gets the actors that are currently selected by this player.
	 * 
	 * @return The actors that are currently selected by this player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	TArray<AActor*> GetSelectedActors() const;

	/**
	 * Causes this player to deselect the specified actor.
	 *
	 * @param Actor The actor to deselect.
	 * @return Whether the actor was deselected successfully. This method will return false if the actor is not selectable
	 *         or was not selected.
	 *
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	bool DeselectActor(AActor* Actor);

	/**
	 * Checks whether the specified actor is selectable by this player.
	 *
	 * @param Actor The actor to test.
	 * @return Whether the actor is selectable by this player.
	 *
	 * @note Actors must have a URiseOwnableComponent to be selectable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	bool IsActorSelectable(const AActor* Actor) const;

	/**
	 * Causes this player to select the specified actor.
	 *
	 * @param Actor The actor to select.
	 * @param bAppend Whether to append the specified actor to the already selected actors instead of overwriting them.
	 * @return Whether the actor was selected successfully. This method will return false if the actor is not selectable
	 *         or is already selected.
	 *
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	bool SelectActor(AActor* Actor, bool bAppend = false);

	/**
	 * Causes this player to select the specified actors.
	 *
	 * @param Actors The actors to select.
	 * @param bAppend Whether to append the specified actors to the already selected actors instead of overwriting them.
	 * @return The number of actors that were selected successfully. Actors that were already selected are not included
	 *         in the result.
	 * 
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	int SelectActors(TArray<AActor*> Actors, bool bAppend = false);

	/**
	 * Casts a ray from the specified screen position and returns the collision results.
	 * 
	 * @param ScreenPosition The screen coordinates to cast the ray from.
	 * @param OutHitResults Reference passed in to store the hit results.
	 * @return Whether the operation completed successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Collision")
	bool GetHitResultsUnderScreenPosition(const FVector2D& ScreenPosition, TArray<FHitResult>& OutHitResults) const;

	/**
	 * Casts a ray from the specified world position and returns the collision results.
	 * 
	 * @param WorldPosition The X and Y world coordinates to cast the ray from.
	 * @param OutHitResults Reference passed in to store the hit results.
	 * @return Whether the operation completed successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Collision")
	bool GetHitResultsUnderWorldPosition(const FVector& WorldPosition, TArray<FHitResult>& OutHitResults) const;

	/**
	 * Casts a ray from the specified world position and returns the collision results.
	 *
	 * @param WorldPosition The X and Y world coordinates to cast the ray from.
	 * @param WorldDirection The direction to cast the ray.
	 * @param OutHitResults Reference passed in to store the hit results.
	 * @return Whether the operation completed successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Collision")
	bool GetHitResultsUnderWorldPositionWithDirection(const FVector& WorldPosition, const FVector& WorldDirection, TArray<FHitResult>& OutHitResults) const;

	/**
	 * Casts a ray from the cursor and returns the collision results.
	 *
	 * @param OutHitResults Reference passed in to store the hit results.
	 * @return Whether the operation completed successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Collision")
	bool GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults) const;

	/**
	 * Casts a box from the selection frame and returns the collision results.
	 *
	 * @param OutHitResults Reference passed in to store the hit results.
	 * @return Whether the operation completed successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Collision")
	bool GetHitResultsUnderSelectionFrame(TArray<FHitResult>& OutHitResults) const;

	/**
	 * Casts a ray from the specified screen position and returns the collision results.
	 * 
	 * @param OutSelectionFrame Reference passed in to store the selection frame rectangle.
	 */
	bool GetSelectionFrame(FIntRect& OutSelectionFrame) const;

	/**
	 * Pans the camera to focus on the specified world location.
	 *
	 * @param NewCameraLocation The world coordinates to focus on.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnWorldLocation2D(const FVector2D& NewCameraLocation);

	/**
	 * Pans the camera to focus on the specified world location.
	 *
	 * @param NewCameraLocation The world coordinates to focus on.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnWorldLocation(const FVector& NewCameraLocation);

	/**
	 * Pans the camera to focus on the specified actor.
	 *
	 * @param Actor The actor to focus on.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnActor(const AActor* Actor);

	/**
	 * Pans the camera to focus on a point which tries to to capture as many of the
	 * provided actors as possible in view.
	 *
	 * @param Actors The actors to focus on.
	 * @param bAllowCameraZoon Whether to allow the camera to zoom in order to capture
	 *                         the necessary units in view.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnActors(TArray<AActor*> Actors, bool bAllowCameraZoom = true);

	/**
	 * Sets whether the camera is allowed to move.
	 * 
	 * @param bEnabled Whether the camera is allowed to move.
	 */
	UFUNCTION(BlueprintCallable)
	void SetCameraMovementEnabled(bool bEnabled);

	/**
	 * Gets whether the camera is allowed to move.
	 * 
	 * @return Whether the camera is allowed to move.
	 */
	UFUNCTION(BlueprintPure)
	bool GetCameraMovementEnabled() const;

	/**
	 * Notifies this player that the specified actor has changed ownership.
	 *
	 * @param Actor The actor that changed ownership.
	 */
	virtual void NotifyActorOwnerChanged(AActor* Actor);

	/**
	 * Event called when an actor has changed ownership.
	 *
	 * @param Actor The actor that changed ownership.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnActorOwnerChanged(AActor* Actor);

	/**
	 * Notifies this player that an error has occurred.
	 *
	 * @param ErrorMessage An error message to display to the player.
	 */
	virtual void NotifyErrorOccurred(const FString& ErrorMessage);

	/**
	 * Event called when an error has occurred.
	 *
	 * @param ErrorMessage An error message to display to the player.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnErrorOccurred(const FString& ErrorMessage);

	/**
	 * Notifies this player that the game has ended.
	 *
	 * @param bIsWinner Whether this player is the winner.
	 */
	virtual void NotifyGameHasEnded(bool bIsWinner);

	/**
	 * Event called when the game has ended.
	 *
	 * @param bIsWinner Whether this player is the winner.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnGameHasEnded(bool bIsWinner);

	/**
	 * Notifies this player that the actor hovered over by this player has changed.
	 *
	 * @param NewHoveredActor The new actor being hovered over.
	 */
	virtual void NotifyHoveredActorChanged(AActor* NewHoveredActor);

	/**
	 * Event called when the actor hovered over by this player has changed.
	 *
	 * @param NewHoveredActor The new actor being hovered over.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnHoveredActorChanged(AActor* NewHoveredActor);

	/**
	 * Notifies this player that the actors selected by this player has changed.
	 *
	 * @param NewSelectedActors The new list of actors that are selected by this player.
	 */
	virtual void NotifySelectedActorsChanged(const TArray<AActor*>& NewSelectedActors);

	/**
	 * Event called when the actors the player has selected has changed.
	 *
	 * @param NewSelectedActors The new list of actors that are selected by this player.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnSelectedActorsChanged(const TArray<AActor*>& NewSelectedActors);

	/**
	 * Notifies this player that their team has changed.
	 *
	 * @param NewTeam The new team the player belongs to.
	 */
	virtual void NotifyTeamChanged(ARiseTeamInfo* NewTeam);

	/**
	 * Event called when the player's team has changed.
	 *
	 * @param NewTeam The new team that the player belongs to.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnTeamChanged(ARiseTeamInfo* NewTeam);

private:

	/**
	 * Causes this player to select the specified actor.
	 *
	 * @param Actor The actor to select.
	 * @param bAppend Whether to append the specified actor to the already selected actors instead of overwriting them.
	 * @param bSuppressNotification Whether to suppress sending the NotifyActorChanged event.
	 * @return Whether the actor was selected successfully. This method will return false if the actor is not selectable
	 *         or is already selected.
	 *
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	bool SelectActor(AActor* Actor, bool bAppend, bool bSuppressNotification);

	/**
	 * Causes this player to select the specified actors.
	 *
	 * @param Actors The actors to select.
	 * @param bAppend Whether to append the specified actors to the already selected actors instead of overwriting them.
	 * @param bSuppressNotification Whether to suppress sending the NotifyActorChanged event.
	 * @return The number of actors that were selected successfully. Actors that were already selected are not included
	 *         in the result.
	 *
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	int SelectActors(TArray<AActor*> Actors, bool bAppend, bool bSuppressNotification);

	/**
	 * Causes this player to deselect the specified actor.
	 *
	 * @param Actor The actor to deselect.
	 * @param bSuppressNotification Whether to suppress sending the NotifyActorChanged event.
	 * @return Whether the actor was deselected successfully. This method will return false if the actor is not selectable
	 *         or was not selected.
	 *
	 * @note Actors must have a URiseOwnableComponent, otherwise they will be ignored.
	 */
	bool DeselectActor(AActor* Actor, bool bSuppressNotification);

public:

	/**
	 * Surrenders the current game.
	 * 
	 * @param bConfirm Whether to request confirmation from the player that they want
	 *                 to surrender.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void Surrender(bool bConfirm = true);

	/**
	 * SERVER: Causes this player to surrender the game.
	 */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSurrender();

	/**
	 * CLIENT: Notifies the client that the game has ended.
	 * 
	 * @param bIsWinner Whether this player is the winner.
	 */
	UFUNCTION(Reliable, Client)
	virtual void ClientGameHasEnded(bool bIsWinner);

protected:

	/**
	 * Event called when the PlayerState is created, replicated, and ready for use.
	 * 
	 * @param NewPlayerState The ready-to-use PlayerState of this player.
	 */
	virtual void OnPlayerStateAvailable(ARisePlayerState* NewPlayerState);

	/**
	 * Starts the selection frame for selecting units.
	 * 
	 * @note This call should be followed by an EndSelectionFrame() call. If this method
	 *       is called again without ending, the old selection starting point will be replaced.
	 */
	UFUNCTION()
	void StartSelectionFrame();

	/**
	 * Ends the selection frame for selecting units and captures the selected actors.
	 */
	UFUNCTION()
	void EndSelectionFrame();

	/**
	 * Marks that the next EndInverseSelectionFrame() call will append or remove a selection instead of replacing
	 * existing selections.
	 */
	UFUNCTION()
	void StartInverseSelectionFrame();

	/**
	 * Resets the StartInverseSelectionFrame() call.
	 */
	UFUNCTION()
	void EndInverseSelectionFrame();

	/**
	 * Returns the possessed pawn cast as an ARisePlayer.
	 * 
	 * @return The possessed pawn cast as an ARisePlayer, or nullptr if the controller is not
	 *         possessing a pawn.
	 */
	FORCEINLINE ARisePlayer* const GetRisePlayer() const;

private:

	float PanCameraHorizontalAxisForce;
	float PanCameraHorizontalDelta;
	float PanCameraVerticalAxisForce;
	float PanCameraVerticalDelta;

	void PanCameraHorizontal(float Scale);
	void PanCameraVertical(float Scale);

	float ZoomCameraCurrentStep;
	float ZoomCameraTargetStep;
	float ZoomCameraDelta;

	void ZoomCameraIn();
	void ZoomCameraOut();

	void UpdateCamera(float DeltaTime);
	float GetCameraZoomActual() const;
	float CalculateCameraMovementSpeed() const;

protected:	

	virtual void OnInput_Debug();
};