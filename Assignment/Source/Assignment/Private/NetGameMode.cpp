// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"
#include "NetBaseCharacter.h"
#include "NetGameState.h"
#include "NetPlayerState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Components/CapsuleComponent.h"

ANetGameMode::ANetGameMode()
{
    DefaultPawnClass = ANetBaseCharacter::StaticClass();
    PlayerStateClass = ANetPlayerState::StaticClass();
    GameStateClass = ANetGameState::StaticClass();
}

AActor* ANetGameMode::GetPlayerStart(FString Name, int Index)
{
    FName PSName;
    if (Index < 0)
    {
        PSName = *Name;
    }
    else
    {
        PSName = *FString::Printf(TEXT("%s%d"), *Name, Index % 4);
    }

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        if (APlayerStart* PS = Cast<APlayerStart>(*It))
        {
            if (PS->PlayerStartTag == PSName) return *It;
        }
    }

    return nullptr;
}

AActor* ANetGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    AActor* Start = AssignTeamAndPlayerStart(Player);
    return Start ? Start : Super::ChoosePlayerStart_Implementation(Player);
}

AActor* ANetGameMode::AssignTeamAndPlayerStart(AController* Player)
{
	AActor* Start = nullptr;
	ANetPlayerState* State = Player->GetPlayerState<ANetPlayerState>();
	if (State)
	{
		if (TotalGames == 0)
		{
			State->TeamID = TotalPlayerCount == 0 ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
			State->PlayerIndex = TotalPlayerCount++;
			AllPlayers.Add(Cast<APlayerController>(Player));
		}
		else
		{
			State->TeamID = State->Result == EGameResults::RESULT_Won ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
		}


		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			Start = GetPlayerStart("Blue", -1);
		}
		else
		{
			Start = GetPlayerStart("Red", PlayerStartIndex++);
		}
	}
	return Start;
}

void ANetGameMode::AvatarsOverlapped(ANetAvatar* AvatarA, ANetAvatar* AvatarB)
{
	ANetGameState* GState = GetGameState<ANetGameState>();

	if (GState == nullptr || GState->WinningPlayer >= 0)return;

	ANetPlayerState* StateA = AvatarA->GetPlayerState<ANetPlayerState>();
	ANetPlayerState* StateB = AvatarB->GetPlayerState<ANetPlayerState>();
	if (StateA->TeamID == StateB->TeamID) return;
	

	if (StateA->TeamID == EPlayerTeam::TEAM_Red)
	{
		GState->WinningPlayer = StateA->PlayerIndex;
	}
	else
	{
		GState->WinningPlayer = StateB->PlayerIndex;
	}

	AvatarA->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AvatarB->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	GState->OnVictory();

	for (APlayerController* Player : AllPlayers)
	{
		auto State = Player->GetPlayerState<ANetPlayerState>();

		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			State->Result = EGameResults::RESULT_Lost;
		}
		else
		{
			State->Result = EGameResults::RESULT_Won;
		}
	}
	FTimerHandle EndGameTimerHandle;
	GWorld->GetTimerManager().SetTimer(EndGameTimerHandle, this, &ANetGameMode::EndGame, 2.5f, false);
}

void ANetGameMode::EndGame()
{
	PlayerStartIndex = 0;
	TotalGames++;
	GetGameState<ANetGameState>()->WinningPlayer = -1;
	for (APlayerController* Player : AllPlayers)
	{
		APawn* Pawn = Player->GetPawn();
		Player->UnPossess();
		Pawn->Destroy();
		Player->StartSpot.Reset();
		RestartPlayer(Player);
	}

	ANetGameState* GState = GetGameState<ANetGameState>();
	GState->TriggerRestart();
}

