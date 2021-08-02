#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/TextProperty.h"
#include "RiseDescriptionComponent.generated.h"

/**
 * Adds a description to an actor. A description can be used to display information
 * about an actor in the UI.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RISE_API URiseDescriptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	/**
	 * Get a description of the owning actor.
	 * 
	 * @return The description of the actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	FText GetDescription() const;

	/**
	 * Sets a description for the owning actor.
	 * 
	 * @param NewDescription The new description of the actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetDescription(const FText& NewDescription);

private:

	/** The description of the owning actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	FText Description;
};
