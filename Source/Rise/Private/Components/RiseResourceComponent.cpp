#include "Components/RiseResourceComponent.h"

#include "Net/UnrealNetwork.h"

#include "RiseMacros.h"

URiseResourceComponent::URiseResourceComponent()
{
	SetIsReplicatedByDefault(true);

	CurrentResourceAmount = 100;
	MaxResourceAmount = CurrentResourceAmount;
	ResourceMultiplier = 1.f;
}

void URiseResourceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URiseResourceComponent, CurrentResourceAmount);
}

void URiseResourceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ResourceClass)
	{
		RISE_ERRORF(TEXT("No resource class assigned to %s."), *GetName());
	}

	//TODO: We may want to do some deeper validation in here.
}

TSubclassOf<URiseResource> URiseResourceComponent::GetResourceType() const
{
	return ResourceClass;
}

int32 URiseResourceComponent::GetMaxResourceAmount() const
{
	return MaxResourceAmount;
}

int32 URiseResourceComponent::GetCurrentResourceAmount() const
{
	return CurrentResourceAmount;
}

int32 URiseResourceComponent::Extract(AActor* Gatherer, int32 DesiredAmount)
{
	if (!Gatherer)
	{
		return 0;
	}

	int32 ModifiedDesiredAmount = FMath::Floor(FMath::Clamp(DesiredAmount, 0, DesiredAmount) * ResourceMultiplier);
	ModifiedDesiredAmount = FMath::Min(ModifiedDesiredAmount, CurrentResourceAmount);

	int32 OldResourceAmount = CurrentResourceAmount;
	int32 NewResourceAmount = CurrentResourceAmount -= ModifiedDesiredAmount;

	UE_LOG(LogRise, Log, TEXT("%s gathered %d %s from %s (%i -> %i)"),
		*Gatherer->GetName(),
		ModifiedDesiredAmount,
		*ResourceClass->GetName(),
		*GetOwner()->GetName(),
		OldResourceAmount,
		NewResourceAmount);

	AActor* Owner = GetOwner();

	OnResourceGathered.Broadcast(Gatherer, Owner, this, ModifiedDesiredAmount);

	if (CurrentResourceAmount <= 0)
	{
		UE_LOG(LogRise, Log, TEXT("%s resource node has been depleted."), *Owner->GetName());

		OnResourceDepleted.Broadcast(Owner, this);

		Owner->Destroy();
	}

	return ModifiedDesiredAmount;
}

bool URiseResourceComponent::CanGatherFromNode_Implementation(AActor* Gatherer) const
{
	// In child classes, we would want to check certain traits of the gatherer.
	// For example, low strength races may not be able to mine stone.
	return true;
}