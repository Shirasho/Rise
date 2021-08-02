#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RisePlayerState.generated.h"

class ARiseTeamInfo;

/**
 * Common player state information.
 */
UCLASS()
class RISE_API ARisePlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	static const uint8 PLAYER_INDEX_NONE;

	ARisePlayerState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/**
	 * Gets the index of the player.
	 * 
	 * @return The index of the player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	uint8 GetPlayerIndex() const;

	/**
	 * Sets the index of the player.
	 * 
	 * @param NewPlayerIndex The new index for this player.
	 */
	void SetPlayerIndex(uint8 NewPlayerIndex);

	/**
	 * Gets the team this player belongs to.
	 * 
	 * @return Gets the information of the team this player belongs to.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Teams")
	ARiseTeamInfo* GetTeam() const;

	/**
	 * Sets the team this player belongs to.
	 * 
	 * @param NewTeam The new team that this player should belong to. Specifying null will remove
	 *                this player from their current team.
	 */
	void SetTeam(ARiseTeamInfo* NewTeam);

	/**
	 * Removes this player from their current team.
	 * 
	 * @note This is the same as passing nullptr to SetTeam().
	 */
	FORCEINLINE void RemoveFromTeam()
	{
		SetTeam(nullptr);
	}

	/**
	 * Checks whether this player belongs to the same team as the specified player.
	 * 
	 * @param Other The player to check the team of.
	 * @return Whether this player belongs to the same team as the specified player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise|Teams")
	bool IsSameTeamAs(const ARisePlayerState* Other) const;

	/**
	 * Returns the actors owned by this player.
	 * 
	 * @return The ownable actors owned by this player.
	 * 
	 * @note All actors in this collection should have a URiseOwnableComponent.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	TArray<AActor*> GetOwnedActors() const;

	/**
	 * Looks for actors this player owns.
	 * 
	 * @note This is an expensive operation.
	 */
	void DiscoverOwnedActors();

	/**
	 * Notifies this player state that the player's team has changed.
	 * 
	 * @param NewTeam Information about the team that this player now belongs to.
	 */
	virtual void NotifyTeamChanged(ARiseTeamInfo* NewTeam);

	/**
	 * The event that gets called when the player's team has changed.
	 *
	 * @param NewTeam Information about the team that this player now belongs to.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise|Teams")
	void OnTeamChanged(ARiseTeamInfo* NewTeam);

	/**
	 * Notifies this player state that ownership of a specific actor has changed.
	 * 
	 * @param AffectedActor The actor whose ownership has changed.
	 * @param OldOwner The player who used to own this actor. A null value means there was no old owner.
	 * @param NewOwner The player who now owns this actor. A null value means this actor is no longer owned by anyone.
	 */
	virtual void NotifyActorOwnershipChanged(AActor* AffectedActor, ARisePlayerState* OldOwner, ARisePlayerState* NewOwner);

	/**
	 * The event that gets called when ownership of an actor has changed.
	 *
	 * @param AffectedActor The actor whose ownership has changed.
	 * @param OldOwner The player who used to own this actor. A null value means there was no old owner.
	 * @param NewOwner The player who now owns this actor. A null value means this actor is no longer owned by anyone.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnActorOwnershipChanged(AActor* AffectedActor, ARisePlayerState* OldOwner, ARisePlayerState* NewOwner);

private:

	/**
	 * The index of the player.
	 */
	UPROPERTY(Replicated)
	uint8 PlayerIndex;

	/**
	 * The team the player is on.
	 */
	UPROPERTY(ReplicatedUsing = OnTeamChangedCallback)
	ARiseTeamInfo* Team;

	/**
	 * The actors this player owns.
	 */
	UPROPERTY()
	TArray<AActor*> OwnedActors;

	UFUNCTION()
	void OnTeamChangedCallback();
};
