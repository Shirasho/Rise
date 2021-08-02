#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "RiseGameplayTagsProvider.h"
#include "RiseActorComponent.generated.h"

/**
 * The base class for all Rise actor components.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RISE_API URiseActorComponent : public UActorComponent, public IRiseGameplayTagsProvider
{
	GENERATED_BODY()

public:

	/**
	 * Adds gameplay tags to the provided tag container.
	 *
	 * @param TagContainer The container to add the tags to.
	 */
	virtual void AddGameplayTags(FGameplayTagContainer& TagContainer) override;
		
protected:

	/** The initial gameplay tags of this actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	FGameplayTagContainer InitialGameplayTags;
};
