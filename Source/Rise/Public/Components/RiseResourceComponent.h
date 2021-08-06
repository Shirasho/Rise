#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RiseResource.h"
#include "RiseResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRiseResourceGatheredSignature, AActor*, Gatherer, AActor*, Source, URiseResourceComponent*, Component, float, GatheredAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRiseResourceDepletedSignature, AActor*, ResourceNode, URiseResourceComponent*, ResourceComponent);

/**
 * When attached to an actor, allows this actor to grant a resource to a player.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RISE_API URiseResourceComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/** The class of resource this node contains. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	TSubclassOf<URiseResource> ResourceClass;

	/** The amount of resources this node contains. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise", meta=(ClampMin = 0))
	int32 MaxResourceAmount;

	/**
	 * The richness of this resource node. This will act as a multiplier for resources gathered.
	 * 
	 * @note This only multiplies the amount of resources gathered at one time. It does not affect
	 * the total number of resources collectable by this resource.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Rise", meta = (ClampMin = 0))
	float ResourceMultiplier;

	/** The number of resources left in this node. */
	UPROPERTY(Replicated)
	float CurrentResourceAmount;

	/** Event called when resources have been gathered from this resource node. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FRiseResourceGatheredSignature OnResourceGathered;

	/** Event called when all resources have been gathered from this resource node. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FRiseResourceDepletedSignature OnResourceDepleted;

public:

	URiseResourceComponent();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:

	/**
	 * Returns the resource type that this node contains.
	 * 
	 * @return The resource type that this node contains.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	TSubclassOf<URiseResource> GetResourceType() const;

	/**
	 * Returns the maximum amount of this resource that this node contains.
	 * 
	 * @return The maximum amount of this resource that this node contains.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	int32 GetMaxResourceAmount() const;

	/**
	 * Returns the current amount of this resource that this node contains.
	 * 
	 * @return The current amount of this resource that this node contains.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	int32 GetCurrentResourceAmount() const;

	/**
	 * Extracts resources from this resource node.
	 * 
	 * @param Gatherer The actor that is gathering resources from this resource node.
	 * @param DesiredAmount The desired amount of resources to gather from this resource node.
	 * @return The actual amount that was gathered from this resource node.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	int32 Extract(AActor* Gatherer, int32 DesiredAmount);

	/**
	 * Checks whether the specified gatherer is able to extract resources from this node.
	 * 
	 * @param Gatherer The actor that wishes to start gathering from this node.
	 * @return Whether the specified actor is able to gather from this resource node.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Rise")
	bool CanGatherFromNode(AActor* Gatherer) const;
};
