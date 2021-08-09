#include "RiseGameMode.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "RiseFeatureFlags.h"
#include "RiseLog.h"
#include "RiseMacros.h"
#include "RisePlayerStart.h"
#include "RiseTeamInfo.h"

ARiseGameMode::ARiseGameMode()
{
	TeamClass = ARiseTeamInfo::StaticClass();
	// In the primary game mode the player is playing against themselves.
	NumTeams = 1;
}

void ARiseGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	if (!TeamClass)
	{
		TeamClass = ARiseTeamInfo::StaticClass();
	}

	for (uint8 TeamIndex = 0; TeamIndex < NumTeams; ++TeamIndex)
	{
		ARiseTeamInfo* Team = GetWorld()->SpawnActor<ARiseTeamInfo>(TeamClass);
		Team->SetTeamIndex(TeamIndex);

		Teams.Add(Team);

		UE_LOG(LogRise, Log, TEXT("Team[%i] %s created."), TeamIndex, *Team->GetName());
	}
}

void ARiseGameMode::BeginPlay()
{
	Super::BeginPlay();

#if RISE_AIPLAYERS_ENABLED
	FString NumAIPlayerString = UGameplayStatics::ParseOption(OptionsString, TEXT("NumAIPlayers"));
	if (!NumAIPlayerString.IsEmpty())
	{
		//TODO: Specify max number of AI players elsewhere.
		NumAIPlayers = FMath::Clamp(FCString::Atoi(*NumAIPlayerString), 0, 7);
	}

	UE_LOG(LogRise, Log, TEXT("Setting the number of AI players to %i"), NumAIPlayers);

	for (int32 PlayerIndex = 0; PlayerIndex < NumAIPlayers; ++PlayerIndex)
	{
		AAIController* AIController = SpawnAIPlayer();
		if (AIController)
		{
			AIController->PlayerState->SetPlayerName(FString::Printf(TEXT("AI Player %i"), PlayerIndex + 1));
		}
	}
#endif RISE_AIPLAYERS_ENABLED
}

void ARiseGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	ARisePlayerStart* PlayerStart = GetRisePlayerStartForPlayer(NewPlayer);
	RestartPlayerAtPlayerStart(NewPlayer, PlayerStart);
}

void ARiseGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	if (!NewPlayer || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	ARisePlayerStart* PlayerStart = Cast<ARisePlayerStart>(StartSpot);
	if (!PlayerStart)
	{
		PlayerStart = GetUnassignedPlayerStart();
		if (PlayerStart)
		{
			PlayerStart->SetPlayer(NewPlayer);
			UE_LOG(LogRise, Log, TEXT("Start spot %s is now occupied by player %s."), *PlayerStart->GetName(), *NewPlayer->GetName());
		}
		else
		{
			RISE_ERRORF(TEXT("Unable to find or assign RisePlayerStart for player %s."), *NewPlayer->GetName());
		}
	}

	AGameModeBase::RestartPlayerAtPlayerStart(NewPlayer, PlayerStart ? PlayerStart : StartSpot);

	ARisePlayerState* PlayerState = Cast<ARisePlayerState>(NewPlayer->PlayerState);
	if (IsValid(PlayerState))
	{
		uint8 PlayerIndex = GetAvailablePlayerIndex();
		PlayerState->SetPlayerIndex(PlayerIndex);

		ARiseTeamInfo* Team = PlayerState->GetTeam();
		if (!Team)
		{
			RISE_WARNING(TEXT("TODO: PlayerState does not have assigned team. This will be set during scenario creation."));
			RISE_WARNING(TEXT("Assigning Player to Team 0."));

			Team = Teams[0];
		}
		Teams[Team->GetTeamIndex()]->AddToTeam(NewPlayer);

		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			URiseOwnableComponent* OwnableComponent = Actor->FindComponentByClass<URiseOwnableComponent>();
			if (IsValid(OwnableComponent) && OwnableComponent->GetInitialOwnerPlayerIndex() == PlayerIndex)
			{
				TransferActorOwnership(Actor, NewPlayer);
			}
		}
	}

	if (!PlayerSpawnParameters.IsEmpty())
	{
		//TODO: We probably don't want to use the PlayerStart's rotation to do this.
		//		We probably need to reach out to the grid manager and detect the
		//		"town center's" rotation on the grid.
		FRotator ActorSpawnRotation(ForceInit);
		ActorSpawnRotation.Yaw = StartSpot->GetActorRotation().Yaw;

		FVector SpawnLocation = StartSpot->GetActorLocation();

		for (int32 SpawnParamIndex = 0; SpawnParamIndex < PlayerSpawnParameters.Num(); ++SpawnParamIndex)
		{
			const FRisePlayerUnitSpawnParameters& SpawnParameters = PlayerSpawnParameters[SpawnParamIndex];

			FVector ActorSpawnLocation = SpawnLocation + SpawnParameters.UnitLocation;
			FTransform ActorSpawnTransform = FTransform(ActorSpawnRotation, ActorSpawnLocation);

			bool bAssignedOwnership;
			AActor* SpawnedActor = SpawnActorForPlayer(SpawnParameters.Unit, NewPlayer, ActorSpawnTransform, bAssignedOwnership);
			if (!bAssignedOwnership)
			{
				UE_LOG(LogRise, Log, TEXT("Unable to assign starting actor %s to player."), *SpawnedActor->GetName());
			}

			//TODO: Depending on how we decide to create buildings, the actor that is spawned may
			//		not be in a usable state. In that case we need to complete construction of
			//      that building. We can do that here.
		}
	}
}

ARisePlayerStart* ARiseGameMode::GetRisePlayerStartForPlayer(AController* Player) const
{
	for (TActorIterator<ARisePlayerStart> It(GetWorld()); It; ++It)
	{
		ARisePlayerStart* PlayerStart = *It;
		if (PlayerStart->GetPlayer() == Player)
		{
			return PlayerStart;
		}
	}

	return nullptr;
}

ARisePlayerStart* ARiseGameMode::GetOrAssignPlayerStartForPlayer(AController* Player)
{
	ARisePlayerStart* FirstEmptySpot = nullptr;

	for (TActorIterator<ARisePlayerStart> It(GetWorld()); It; ++It)
	{
		ARisePlayerStart* PlayerStart = *It;
		if (PlayerStart->GetPlayer() == Player)
		{
			return PlayerStart;
		}
		else if (PlayerStart->GetPlayer() == nullptr)
		{
			FirstEmptySpot = PlayerStart;
		}
	}

	if (FirstEmptySpot)
	{
		FirstEmptySpot->SetPlayer(Player);
	}

	return FirstEmptySpot;
}

ARisePlayerStart* ARiseGameMode::GetUnassignedPlayerStart() const
{
	for (TActorIterator<ARisePlayerStart> It(GetWorld()); It; ++It)
	{
		ARisePlayerStart* PlayerStart = *It;
		if (PlayerStart->GetPlayer() == nullptr)
		{
			return PlayerStart;
		}
	}

	return nullptr;
}

TArray<ARiseTeamInfo*> ARiseGameMode::GetTeams() const
{
	return Teams;
}

uint8 ARiseGameMode::GetAvailablePlayerIndex()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return ARisePlayerState::PLAYER_INDEX_NONE;
	}

	// TODO: There's gotta be a better way to do this.
	TArray<const ARisePlayerState*> PlayerStates;
	for (TActorIterator<ARisePlayerState> It(World); It; ++It)
	{
		PlayerStates.Add(*It);
	}

	Algo::SortBy(PlayerStates, &ARisePlayerState::GetPlayerIndex);

	for (int32 PlayerIndex = 0; PlayerIndex < ARisePlayerState::PLAYER_INDEX_NONE; ++PlayerIndex)
	{
		// If we have exceeded the length of our array the condition
		// below *must* have failed in every indice, meaning there are
		// no discrepencies between the player's index and their index
		// in the state array. In that case go ahead and add them to the end.
		if (!PlayerStates.IsValidIndex(PlayerIndex))
		{
			return PlayerIndex;
		}

		// If the player's index does not match their index in the player
		// state array, that player state array index is free to use as
		// a player state index.
		if (PlayerStates[PlayerIndex]->GetPlayerIndex() != PlayerIndex)
		{
			return PlayerIndex;
		}
	}

	// We have no room for them.
	return ARisePlayerState::PLAYER_INDEX_NONE;
}

AAIController* ARiseGameMode::SpawnAIPlayer()
{
#if RISE_AIPLAYERS_ENABLED
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AIPlayerControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
	if (AIController)
	{
		// Perform any necessary initializations here.

		UGameplayStatics::FinishSpawningActor(AIController, FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
		UE_LOG(LogRise, Log, TEXT("Spawned AI player %s."), *AIController->GetName());
	}
	else
	{
		UE_LOG(LogRise, Error, TEXT("Failed to spawn AI player."));
	}

	RestartPlayer(AIController);

	return AIController;
#else
	unimplemented();
	return nullptr;
#endif
}

AActor* ARiseGameMode::SpawnActorForPlayer(TSubclassOf<AActor> ActorClass, AController* ActorOwner, const FTransform& SpawnTransform, bool& bOutActorAssignedOwnership)
{
	bOutActorAssignedOwnership = false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass->GetDefaultObject()->GetClass(), SpawnTransform, SpawnParams);
	if (SpawnedActor && ActorOwner)
	{
		UE_LOG(LogRise, Log, TEXT("Player %s spawned unit %s at location %s"), *SpawnedActor->GetName(), *ActorOwner->GetName(), *SpawnTransform.GetLocation().ToString());

		bOutActorAssignedOwnership = TransferActorOwnership(SpawnedActor, ActorOwner);
	}

	return SpawnedActor;
}

bool ARiseGameMode::TransferActorOwnership(AActor* Actor, AController* NewOwner)
{
	if (!Actor)
	{
		return false;
	}

	URiseOwnableComponent* OwnableComponent = Actor->FindComponentByClass<URiseOwnableComponent>();
	ARisePlayerState* OldOwnerState = nullptr;
	ARisePlayerState* NewOwnerState = NewOwner ? Cast<ARisePlayerState>(NewOwner->PlayerState) : nullptr;

	if (OwnableComponent)
	{
		OldOwnerState = OwnableComponent->GetPlayerOwner();
	}

	if (OldOwnerState == NewOwnerState)
	{
		return false;
	}

	if (!IsValid(OldOwnerState))
	{
		UE_LOG(LogRise, Log, TEXT("Transferred ownership of %s to %s."), *Actor->GetName(), *NewOwner->GetName());
	}
	else
	{
		UE_LOG(LogRise, Log, TEXT("Transferred ownership of %s from %s to %s."), *Actor->GetName(), *OldOwnerState->GetName(), *NewOwner->GetName());
	}

	Actor->SetOwner(NewOwner);
	OwnableComponent->SetPlayerOwnerByController(NewOwner);

	//TODO: If the player has GodMode enabled, set it here.
	//APawn* Pawn = Cast<APawn>(Actor);
	//if (Pawn)
	//{
	//	Pawn->SetCanBeDamaged(true);
	//}

	return true;
}

void ARiseGameMode::NotifyActorKilled(AActor* Actor, AController* ActorOwner)
{
	if (ActorOwner)
	{
		URiseOwnableComponent* OwnableComponent = Actor->FindComponentByClass<URiseOwnableComponent>();
		if (IsValid(OwnableComponent))
		{
			ARisePlayerState* PlayerState = OwnableComponent->GetPlayerOwner();
			if (PlayerState)
			{
				ARisePlayerState* ActorOwnerPlayerState = Cast<ARisePlayerState>(ActorOwner->PlayerState);
				if (PlayerState == ActorOwnerPlayerState)
				{
					// The passed-in actor owner does indeed own the actor.
					OnActorKilled(Actor, ActorOwner);
					return;
				}
			}
		}
	}

	// We could not verify that the passed-in actor owner 
	OnActorKilled(Actor, nullptr);
}

void ARiseGameMode::OnActorKilled_Implementation(AActor* Actor, AController* ActorOwner)
{
	for (FRiseDefeatConditionBase& DefeatCondition : DefeatConditions)
	{
		if (DefeatCondition.IsDefeated(Actor, ActorOwner))
		{
			UE_LOG(LogRise, Log, TEXT("Player %s was defeated - %s"), *ActorOwner->GetName(), *DefeatCondition.GetDefeatReason());
			NotifyPlayerDefeated(ActorOwner);
			break;
		}
	}
}

void ARiseGameMode::NotifyPlayerResigned(AController* Player)
{
	OnPlayerResigned(Player);
}

void ARiseGameMode::OnPlayerResigned_Implementation(AController* Player)
{

}

void ARiseGameMode::NotifyPlayerDefeated(AController* Player)
{
	OnPlayerDefeated(Player);
}

void ARiseGameMode::OnPlayerDefeated_Implementation(AController* Player)
{

}