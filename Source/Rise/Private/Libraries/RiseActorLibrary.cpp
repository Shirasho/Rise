#include "Libraries/RiseActorLibrary.h"

#include "RisePlayerState.h"
#include "Components/RiseOwnableComponent.h"

bool URiseActorLibrary::IsActorOwnedByLocalPlayer(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	return IsOwnedByLocalPlayer(Actor->FindComponentByClass<URiseOwnableComponent>());
}

bool URiseActorLibrary::IsOwnedByLocalPlayer(const URiseOwnableComponent* OwnableComponent)
{
	if (!IsValid(OwnableComponent))
	{
		return false;
	}

	ARisePlayerState* Owner = OwnableComponent->GetPlayerOwner();
	if (!Owner)
	{
		return false;
	}

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	APlayerController* OwnerController = World->GetFirstPlayerController();
	if (!IsValid(OwnerController))
	{
		return false;
	}

	return Owner == OwnerController->GetPlayerState<ARisePlayerState>();
}

bool URiseActorLibrary::IsActorOwnedByAI(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	return IsOwnedByAI(Actor->FindComponentByClass<URiseOwnableComponent>());
}

bool URiseActorLibrary::IsOwnedByAI(const URiseOwnableComponent* OwnableComponent)
{
	if (!IsValid(OwnableComponent))
	{
		return false;
	}

	ARisePlayerState* PlayerState = OwnableComponent->GetPlayerOwner();
	return PlayerState && PlayerState->IsABot();
}