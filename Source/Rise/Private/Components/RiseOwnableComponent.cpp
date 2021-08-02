#include "Components/RiseOwnableComponent.h"

#include "Net/UnrealNetwork.h"

#include "RisePlayerState.h"

URiseOwnableComponent::URiseOwnableComponent()
{
	SetIsReplicatedByDefault(true);

	InitialOwnerPlayerIndex = ARisePlayerState::PLAYER_INDEX_NONE;
}

void URiseOwnableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URiseOwnableComponent, Owner);
}

ARisePlayerState* URiseOwnableComponent::GetPlayerOwner() const
{
	return Owner;
}

void URiseOwnableComponent::SetPlayerOwnerByController(AController* NewOwner)
{
	if (!IsValid(NewOwner))
	{
		SetPlayerOwnerByPlayerState(nullptr);
	}
	else
	{
		SetPlayerOwnerByPlayerState(Cast<ARisePlayerState>(NewOwner->PlayerState));
	}
}

void URiseOwnableComponent::SetPlayerOwnerByPlayerState(ARisePlayerState* NewOwner)
{
	ARisePlayerState* OldOwner = Owner;
	Owner = NewOwner;

	if (OldOwner != NewOwner)
	{
		NotifyOwnerChanged(OldOwner, NewOwner);
	}
}

bool URiseOwnableComponent::IsSameTeamAsActor(AActor* Other) const
{
	// No-team actors do not share a team.
	if (!IsValid(Other) || !Owner)
	{
		return false;
	}

	URiseOwnableComponent* OtherOwnerComponent = Other->FindComponentByClass<URiseOwnableComponent>();
	if (!OtherOwnerComponent)
	{
		return false;
	}

	return Owner->IsSameTeamAs(OtherOwnerComponent->Owner);
}

bool URiseOwnableComponent::IsSameTeamAsController(AController* Other) const
{
	if (!IsValid(Other))
	{
		return false;
	}

	return Owner && Owner->IsSameTeamAs(Cast<ARisePlayerState>(Other->PlayerState));
}

uint8 URiseOwnableComponent::GetInitialOwnerPlayerIndex() const
{
	return InitialOwnerPlayerIndex;
}

void URiseOwnableComponent::OnOwnerChangedCallback(ARisePlayerState* OldOwner)
{
	NotifyOwnerChanged(OldOwner, Owner);
}

void URiseOwnableComponent::NotifyOwnerChanged(ARisePlayerState* OldOwner, ARisePlayerState* NewOwner)
{
	OnOwnerChanged(GetOwner(), NewOwner ? Cast<AController>(NewOwner->GetOwner()) : nullptr);

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		for (FConstControllerIterator ControllerIt = World->GetControllerIterator(); ControllerIt; ++ControllerIt)
		{
			TWeakObjectPtr<AController> Controller = *ControllerIt;
			ARisePlayerState* PlayerState = Controller->GetPlayerState<ARisePlayerState>();

			if (IsValid(PlayerState) && (PlayerState == OldOwner || PlayerState == NewOwner))
			{
				PlayerState->NotifyActorOwnershipChanged(GetOwner(), OldOwner, NewOwner);
			}
		}
	}
}
