#include "RiseTeamInfo.h"

#include "Net/UnrealNetwork.h"

#include "RisePlayerState.h"

ARiseTeamInfo::ARiseTeamInfo()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 1.f;

	TeamIndex = 255;
}

void ARiseTeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARiseTeamInfo, TeamIndex, COND_InitialOnly);
}

void ARiseTeamInfo::AddToTeam(AController* Player)
{
	if (!Player)
	{
		return;
	}

	// If the player is already on this team, return early.
	if (TeamPlayers.Contains(Player))
	{
		return;
	}

	ARisePlayerState* PlayerState = Cast<ARisePlayerState>(Player->PlayerState);
	if (!PlayerState)
	{
		return;
	}

	// The player can only be added to one team at a time, so if they are already
	// on a team, remove them from it.
	if (PlayerState->GetTeam())
	{
		RemoveFromTeam(Player);
	}

	// Add the player to the team.
	PlayerState->SetTeam(this);
	TeamPlayers.Add(Player);

	// Send off notifications.
	PlayerState->NotifyTeamChanged(this);
}

void ARiseTeamInfo::RemoveFromTeam(AController* Player)
{
	if (!Player)
	{
		return;
	}

	// If the player is not in the team, return early.
	if (!TeamPlayers.Contains(Player))
	{
		return;
	}

	// Remove the player from the team.
	TeamPlayers.Remove(Player);

	ARisePlayerState* PlayerState = Cast<ARisePlayerState>(Player->PlayerState);
	if (!PlayerState)
	{
		return;
	}

	PlayerState->SetTeam(nullptr);
	PlayerState->NotifyTeamChanged(nullptr);
}

bool ARiseTeamInfo::IsOnTeam(AController* Player) const
{
	if (!Player)
	{
		return false;
	}

	return TeamPlayers.Contains(Player);
}

uint8 ARiseTeamInfo::GetTeamIndex() const
{
	return TeamIndex;
}

void ARiseTeamInfo::SetTeamIndex(uint8 NewTeamIndex)
{
	TeamIndex = NewTeamIndex;
}

TArray<AController*> ARiseTeamInfo::GetTeamPlayers() const
{
	return TeamPlayers;
}