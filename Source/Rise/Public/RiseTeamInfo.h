#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "RiseTeamInfo.generated.h"

/**
 * Generic information about a team in most game modes.
 */
UCLASS()
class RISE_API ARiseTeamInfo : public AInfo
{
	GENERATED_BODY()
	
public:

	ARiseTeamInfo();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/**
	 * Adds the specified player to this team.
	 * 
	 * @param Player The player to add to the team.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	virtual void AddToTeam(AController* Player);

	/**
	 * Removes the specified player from this team.
	 * 
	 * @param Player The player to remove from the team.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	virtual void RemoveFromTeam(AController* Player);

	/**
	 * Checks whether the specified player is on this team.
	 * 
	 * @param Player The player to check for.
	 * @return Whether the specified player is on this team.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	bool IsOnTeam(AController* Player) const;

	/**
	 * Gets the players on this team.
	 * 
	 * @return The players on this team.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	TArray<AController*> GetTeamPlayers() const;

	/**
	 * Gets the index of this team.
	 * 
	 * @return The index of this team.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	uint8 GetTeamIndex() const;

	/**
	 * Sets the index of this team.
	 * 
	 * @param NewTeamIndex The new index of this team.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetTeamIndex(uint8 NewTeamIndex);

private:

	/** The index of this team. */
	UPROPERTY(Replicated)
	uint8 TeamIndex;

	/** The players on this team. */
	UPROPERTY()
	TArray<AController*> TeamPlayers;
};
