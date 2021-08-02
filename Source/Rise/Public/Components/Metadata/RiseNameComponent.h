#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/TextProperty.h"

#include "RiseNameComponent.generated.h"


/**
 * Adds a name to an actor. A name can be used to display information
 * about an actor in the UI.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RISE_API URiseNameComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/** The name of the owning actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
		FText Name;

	/** The user-provided custom name of the owning actor. */
	UPROPERTY(ReplicatedUsing = OnCustomNameChangedCallback)
		FString CustomName;

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:

	/** Event called when the custom name has changed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Rise")
	void OnCustomNameChanged(const FString& NewCustomName);

public:

	/**
	 * Get a name of the owning actor.
	 * 
	 * @return The name of the owning actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	FText GetName() const;

	/**
	 * Sets a name for the owning actor.
	 * 
	 * @param NewName The new name of the owning actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetName(const FText& NewName);

	/**
	 * Get a user-provided name of the owning actor.
	 *
	 * @return The custom name of the owning actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	FString GetCustomName() const;

	/**
	 * Sets a user-provided name for the owning actor.
	 *
	 * @param NewName The new custom name of the owning actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetCustomName(const FString& NewCustomName);

private:

	UFUNCTION()
	void OnCustomNameChangedCallback(FString NewCustomName);
};