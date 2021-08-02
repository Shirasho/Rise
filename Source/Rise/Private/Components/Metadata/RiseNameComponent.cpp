#include "Components/Metadata/RiseNameComponent.h"

#include "Net/UnrealNetwork.h"

void URiseNameComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URiseNameComponent, Name);
}

FText URiseNameComponent::GetName() const
{
	return Name;
}

void URiseNameComponent::SetName(const FText& NewName)
{
	Name = NewName;
}

FString URiseNameComponent::GetCustomName() const
{
	return CustomName;
}

void URiseNameComponent::SetCustomName(const FString& NewCustomName)
{
	CustomName = NewCustomName;
}

void URiseNameComponent::OnCustomNameChangedCallback(FString NewCustomName)
{
	OnCustomNameChanged(NewCustomName);
}