#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RiseResource.generated.h"

class UTexture2D;

/**
 * The base class for a resource that can be collected by a player.
 */
UCLASS(Blueprintable, BlueprintType)
class RISE_API URiseResource : public UObject
{
	GENERATED_BODY()

private:

	/** The name of the resource. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	FText ResourceName;

	/** The resource icon texture. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	UTexture2D* ResourceIcon;

	/** Gameplay tags for this resource. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	FGameplayTagContainer ResourceTags;

public:

	/**
	 * Returns the resource's name.
	 *
	 * @return The resource's name.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	FText GetResourceName() const;

	/**
	 * Returns the resource's icon texture.
	 *
	 * @return The resource's icon texture.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	UTexture2D* GetResourceIcon() const;

	/**
	 * Returns the tags for this resource.
	 *
	 * @return The tags for this resource.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	FGameplayTagContainer GetResourceTags() const;
};