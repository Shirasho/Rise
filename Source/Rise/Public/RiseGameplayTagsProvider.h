#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"

#include "RiseGameplayTagsProvider.generated.h"

/**
 * Provides gameplay tags for an actor.
 */
UINTERFACE(MinimalAPI)
class URiseGameplayTagsProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * Provides gameplay tags for an actor.
 */
class RISE_API IRiseGameplayTagsProvider
{
	GENERATED_BODY()

public:

	/**
	 * Adds gameplay tags to the provided tag container.
	 * 
	 * @param TagContainer The container to add the tags to.
	 */
	virtual void AddGameplayTags(FGameplayTagContainer& TagContainer);
};
