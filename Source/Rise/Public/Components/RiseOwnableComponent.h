#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RiseOwnableComponent.generated.h"

class AController;
class ARisePlayerState;

/**
 * When attached to an actor, allows a player to own that actor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RISE_API URiseOwnableComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/** The index of the player that should initially own the actor. */
	UPROPERTY(EditInstanceOnly, Category = "Rise")
	uint8 InitialOwnerPlayerIndex;

	/** The player that owns this actor. */
	UPROPERTY(ReplicatedUsing = OnOwnerChangedCallback)
	ARisePlayerState* Owner;

public:	
	URiseOwnableComponent();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Gets the player that owns this actor.
	 * 
	 * @return The player that owns this actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	ARisePlayerState* GetPlayerOwner() const;

	/**
	 * Sets the player that owns this actor.
	 * 
	 * @param NewOwner The new owner of this actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetPlayerOwnerByController(AController* NewOwner);

	/**
	 * Sets the player that owns this actor.
	 * 
	 * @param NewOwner The new owner of this actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetPlayerOwnerByPlayerState(ARisePlayerState* NewOwner);

	/**
	 * Checks whether the actor is owned by the same player as the specified actor.
	 * 
	 * @param Other The actor to compare teams with.
	 * @return Whether the actor is owned by the same player as the specified actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	bool IsSameTeamAsActor(AActor* Other) const;

	/**
	 * Checks whether the actor is owned by the player with the specified PlayerController.
	 * 
	 * @param The player to compare teams with.
	 * @return Whether the actor is owned by the specified player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	bool IsSameTeamAsController(AController* Other) const;

	/**
	 * Gets the index of the player that should initially own the actor.
	 * 
	 * @return The index of the player this actor originally belongs to.
	 */
	uint8 GetInitialOwnerPlayerIndex() const;

	/**
	 * Event called when the owner of the actor has changed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnOwnerChanged(AActor* Actor, AController* NewOwner);

private:

	UFUNCTION()
	void OnOwnerChangedCallback(ARisePlayerState* NewOwner);

	/**
	 * Notifies that the owner of the actor has changed.
	 */
	void NotifyOwnerChanged(ARisePlayerState* OldOwner, ARisePlayerState* NewOwner);
};
