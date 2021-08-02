#include "RisePlayerState.h"

#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "RisePlayerController.h"
#include "RiseTeamInfo.h"
#include "RiseLog.h"
#include "Components/RiseOwnableComponent.h"

const uint8 ARisePlayerState::PLAYER_INDEX_NONE = 255;

ARisePlayerState::ARisePlayerState()
{
	PlayerIndex = PLAYER_INDEX_NONE;
}

void ARisePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARisePlayerState, PlayerIndex);
	DOREPLIFETIME(ARisePlayerState, Team);
}

uint8 ARisePlayerState::GetPlayerIndex() const
{
	return PlayerIndex;
}

void ARisePlayerState::SetPlayerIndex(uint8 NewPlayerIndex)
{
	PlayerIndex = NewPlayerIndex;
}

ARiseTeamInfo* ARisePlayerState::GetTeam() const
{
	return Team;
}

void ARisePlayerState::SetTeam(ARiseTeamInfo* NewTeam)
{
	Team = NewTeam;
}

bool ARisePlayerState::IsSameTeamAs(const ARisePlayerState* Other) const
{
	if (!Other)
	{
		return false;
	}

	ARiseTeamInfo* OtherTeam = Other->Team;

	// If two players are not on a team, they are not considered to be on the
	// same team.
	if (!Team || !Other->Team)
	{
		return false;
	}

	return Team->GetTeamIndex() == OtherTeam->GetTeamIndex();
}

TArray<AActor*> ARisePlayerState::GetOwnedActors() const
{
	return OwnedActors;
}

void ARisePlayerState::DiscoverOwnedActors()
{
	OwnedActors.Empty();

	for (TActorIterator<AActor> ActorIt(GetWorld()); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;

		if (!IsValid(Actor))
		{
			continue;
		}

		URiseOwnableComponent* OwnableComponent = Actor->FindComponentByClass<URiseOwnableComponent>();
		if (IsValid(OwnableComponent) && OwnableComponent->GetOwner() == this)
		{
			OwnedActors.AddUnique(Actor);
		}
	}
}

void ARisePlayerState::NotifyTeamChanged(ARiseTeamInfo* NewTeam)
{
	if (NewTeam)
	{
		UE_LOG(LogRise, Log, TEXT("Player %s changed to team %s"), *GetName(), *NewTeam->GetName());
	}
	else
	{
		UE_LOG(LogRise, Log, TEXT("Player % is no longer on a team."), *GetName());
	}

	OnTeamChanged(NewTeam);

	//TODO: Do we need AI controllers to be aware of this?
	ARisePlayerController* PlayerController = Cast<ARisePlayerController>(GetOwner());
	if (PlayerController)
	{
		PlayerController->NotifyTeamChanged(NewTeam);
	}
}

void ARisePlayerState::OnTeamChangedCallback()
{
	NotifyTeamChanged(Team);
}

void ARisePlayerState::NotifyActorOwnershipChanged(AActor* AffectedActor, ARisePlayerState* OldOwner, ARisePlayerState* NewOwner)
{
	if (OldOwner == NewOwner || !AffectedActor)
	{
		return;
	}

	if (NewOwner == this)
	{
		OwnedActors.AddUnique(AffectedActor);
	}
	else
	{
		OwnedActors.Remove(AffectedActor);
	}

	//TODO: Do we need AI controllers to be aware of this?
	ARisePlayerController* PlayerController = Cast<ARisePlayerController>(GetOwner());
	if (PlayerController)
	{
		PlayerController->NotifyActorOwnerChanged(AffectedActor);
	}

	OnActorOwnershipChanged(AffectedActor, OldOwner, NewOwner);
}