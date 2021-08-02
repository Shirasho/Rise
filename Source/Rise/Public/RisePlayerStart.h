#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "RisePlayerStart.generated.h"

class ARiseGameMode;

/**
 * A PlayerStart for Rise players.
 */
UCLASS()
class RISE_API ARisePlayerStart : public APlayerStart
{
	GENERATED_BODY()

private:

	/** The player that spawned at this start point. */
	UPROPERTY()
	AController* Player;

public:

	/**
	 * Gets the player that spawned at this start point.
	 * 
	 * @return The player that spawned at this start point.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	AController* GetPlayer() const;

private:

	/**
	 * Sets the player who will spawn at this start point.
	 * 
	 * @param NewPlayer The player to spawn at this start point.
	 */
	void SetPlayer(AController* NewPlayer);

	friend class ARiseGameMode;
};