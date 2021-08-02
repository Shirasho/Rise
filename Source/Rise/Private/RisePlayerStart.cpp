#include "RisePlayerStart.h"

AController* ARisePlayerStart::GetPlayer() const
{
	return Player;
}

void ARisePlayerStart::SetPlayer(AController* NewPlayer)
{
	Player = NewPlayer;
}