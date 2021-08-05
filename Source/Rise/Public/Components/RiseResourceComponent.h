#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RiseResourceComponent.generated.h"

/**
 * When attached to an actor, allows this actor to grant a resource to a player.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RISE_API URiseResourceComponent : public UActorComponent
{
	GENERATED_BODY()
};
