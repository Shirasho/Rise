#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Templates/SubclassOf.h"

#include "RisePlayerState.h"
#include "Components/RiseOwnableComponent.h"
#include "RiseGameMode.generated.h"

/**
 * A structure that defines how to spawn units.
 */
USTRUCT()
struct FRisePlayerUnitSpawnParameters
{
	GENERATED_USTRUCT_BODY()

public:

	//TODO: Replace this class with the base class for spawnable units.
	/** The class of the actor to spawn. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> Unit;
	
	/** The local offset location to spawn this unit. */
	UPROPERTY(EditDefaultsOnly)
	FVector UnitLocation;
};

/**
 * The base class for defeat conditions.
 */
USTRUCT()
struct FRiseDefeatConditionBase
{
	GENERATED_USTRUCT_BODY()

public:

	/**
	 * Checks whether the deletion of the provided actor would cause a
	 * defeat for the provided player.
	 *
	 * @param AActor The actor that was just killed.
	 * @param ActorOwner The owner of the actor that was just killed.
	 * @return Whether the player is defeated.
	 */
	virtual bool IsDefeated(AActor* Actor, AController* ActorOwner)
	{
		return false;
	}

	/**
	 * Returns the reason why the player was defeated.
	 *
	 * @return The reason why the player was defeated.
	 */
	virtual FString GetDefeatReason()
	{
		return TEXT("");
	}
};

/**
 * A defeat condition that occurs when the player does not have the necessary number of
 * a specific actor under their control.
 */
USTRUCT()
struct FRiseRequiredUnitsDefeatCondition : public FRiseDefeatConditionBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** The type of actor that must be under this player's control. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorClass;

	/** The number of this type of actor that must be under this player's control. */
	UPROPERTY(EditDefaultsOnly)
	int32 ActorCount;

public:

	/**
	 * Checks whether the deletion of the provided actor would cause a
	 * defeat for the provided player.
	 * 
	 * @param AActor The actor that was just killed.
	 * @param ActorOwner The owner of the actor that was just killed.
	 * @return Whether the player is defeated.
	 */
	bool IsDefeated(AActor* Actor, AController* ActorOwner) override
	{
		if (!Actor)
		{
			return false;
		}

		// Double check that the actor owner is valid.
		ARisePlayerState* PlayerState = nullptr;
		if (ActorOwner)
		{
			PlayerState = Cast<ARisePlayerState>(ActorOwner->PlayerState);
		}
		else
		{
			URiseOwnableComponent* OwnableComponent = Actor->FindComponentByClass<URiseOwnableComponent>();
			if (IsValid(OwnableComponent))
			{
				PlayerState = OwnableComponent->GetPlayerOwner();
			}
		}

		// If we can't get the owner we cannot assume the player is defeated.
		if (!IsValid(PlayerState))
		{
			return false;
		}

		int32 TargetOwnedActors = 0;
		for (AActor* OwnedActor : PlayerState->GetOwnedActors())
		{
			if (OwnedActor->GetClass()->IsChildOf(ActorClass))
			{
				++TargetOwnedActors;
			}
		}

		return TargetOwnedActors >= ActorCount;
	}

	/**
	 * Returns the reason why the player was defeated.
	 * 
	 * @return The reason why the player was defeated.
	 */
	virtual FString GetDefeatReason() override
	{
		return TEXT("The player does not control the necessary actors.");
	}
};

class AAIController;
class ARisePlayerStart;
class ARiseTeamInfo;

/**
 * Common game mode information.
 */
UCLASS()
class RISE_API ARiseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ARiseGameMode();

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;


private:

	/** The subclass of TeamInfo to use. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	TSubclassOf<ARiseTeamInfo> TeamClass;

	/** Parameters describing what units each player starts the game with. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	TArray<FRisePlayerUnitSpawnParameters> PlayerSpawnParameters;

	/** 
	 * Conditions that would cause a player to lose the game.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	TArray<FRiseDefeatConditionBase> DefeatConditions;

	/** The number of teams in this game mode. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	uint8 NumTeams;

	/** The teams of the current game. */
	UPROPERTY()
	TArray<ARiseTeamInfo*> Teams;

	/** The class of the AIController to use for AI players. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|AI")
	TSubclassOf<AAIController> AIPlayerControllerClass;

	/** The number of AI players to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|AI")
	uint8 NumAIPlayers;

public:

	/** 
	 * Spawns an AI player.
	 * 
	 * @return The AI player's controller.
	 */
	virtual AAIController* SpawnAIPlayer();

	/**
	 * Spawns an actor and assigns ownership to the specified player.
	 * 
	 * @param ActorClass The class of the actor to spawn.
	 * @param ActorOwner The Controller of the player who is to receive ownership of the actor.
	 * @param SpawnTransform The transform of the actor to spawn.
	 * @param bOutActorOwnership Reference passed in to store whether the player successfully gained ownership of the actor.
	 * @return The spawned actor.
	 * 
	 * @note If the ActorClass is not ownable it will be spawned with no ownership.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	virtual AActor* SpawnActorForPlayer(TSubclassOf<AActor> ActorClass, AController* ActorOwner, const FTransform& SpawnTransform, bool& bOutActorAssignedOwnership);

	/**
	 * Attempts to transfer ownership of the specified actor to the specified player.
	 * 
	 * @param Actor The actor to change the owner of.
	 * @param NewOwner The player to assume control of the specified actor.
	 * @return Whether ownership was successfully transferred.
	 * 
	 * @note If the actor is not ownable or the player already owns the actor this method will return false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	bool TransferActorOwnership(AActor* Actor, AController* NewOwner);

	/**
	 * Finds the PlayerStart for the specified player.
	 * 
	 * @param Player The player to find the PlayerStart for.
	 * @return The PlayerStart associated with the player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	virtual ARisePlayerStart* GetRisePlayerStartForPlayer(AController* Player) const;

	/**
	 * Gets the teams of the current match.
	 * 
	 * @return The teams of the current match.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	TArray<ARiseTeamInfo*> GetTeams() const;

	/**
	 * Notifies the GameMode that an actor has been killed.
	 * 
	 * @param Actor The actor that was killed.
	 * @param ActorOwner The owner of the actor.
	 * 
	 * @note The ActorOwner will be verified against the OwnableComponent
	 *       of the Actor. If they do not match, the ActorOwner will be
	 *       ignored.
	 */
	void NotifyActorKilled(AActor* Actor, AController* ActorOwner);

	/**
	 * Notifies the GameMode that a player has been defeated.
	 *
	 * @param Player The player that was defeated.
	 */
	void NotifyPlayerDefeated(AController* Player);

	/**
	 * Notifies the GameMode that a player has resigned.
	 *
	 * @param Player The player that resigned.
	 */
	void NotifyPlayerResigned(AController* Player);

	/**
	 * Event called when an actor is killed.
	 * 
	 * @param Actor The actor that was killed.
	 * @param ActorOwner The owner of the actor that was killed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Rise")
	void OnActorKilled(AActor* Actor, AController* ActorOwner);

	/**
	 * Event called when a player has been defeated.
	 *
	 * @param Player The player that was defeated.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Rise")
	void OnPlayerDefeated(AController* Player);

	/**
	 * Event called when a player has resigned.
	 *
	 * @param Player The player that resigned.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Rise")
	void OnPlayerResigned(AController* Player);

protected:

	/**
	 * Returns the index of the first player slot that isn't assigned to a player.
	 * 
	 * @return The index of the first player slot that isn't assigned to a player.
	 */
	uint8 GetAvailablePlayerIndex();
};
