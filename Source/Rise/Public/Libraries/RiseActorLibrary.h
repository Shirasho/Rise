#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "RiseActorLibrary.generated.h"

class URiseOwnableComponent;

UCLASS()
class RISE_API URiseActorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Checks whether the specified actor is owned by the local player.
	 * 
	 * @param Actor The actor to check the ownership of.
	 * @return Whether the actor is owned by the local player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	static bool IsActorOwnedByLocalPlayer(const AActor* Actor);

	/**
	 * Checks whether the owner of the specified OwnableComponent is owned by the local player.
	 *
	 * @param Actor The OwnableComponent to check the ownership of.
	 * @return Whether the owner of the specified OwnableComponent is owned by the local player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	static bool IsOwnedByLocalPlayer(const URiseOwnableComponent* OwnableComponent);

	/**
	 * Checks whether the specified actor is owned by an AI player.
	 *
	 * @param Actor The actor to check the ownership of.
	 * @return Whether the actor is owned by an AI player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	static bool IsActorOwnedByAI(const AActor* Actor);

	/**
	 * Checks whether the owner of the specified OwnableComponent is owned by an AI player.
	 *
	 * @param Actor The OwnableComponent to check the ownership of.
	 * @return Whether the owner of the specified OwnableComponent is owned by an AI player.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	static bool IsOwnedByAI(const URiseOwnableComponent* OwnableComponent);
};