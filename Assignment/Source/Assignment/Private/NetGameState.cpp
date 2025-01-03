// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include "NetGameState.h"
#include "NetBaseCharacter.h"
#include "NetPlayerState.h"
#include "Net/UnrealNetwork.h"

ANetGameState::ANetGameState() :
	WinningPlayer(-1)
{
}

void ANetGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetGameState, WinningPlayer);
	DOREPLIFETIME(ANetGameState, TimeLeft);
	DOREPLIFETIME(ANetGameState, IsTimeLeft);
}

void ANetGameState::OnRep_Winner()
{
	if (WinningPlayer >= 0 || TimeLeft <= 0)
	{
		if (TimeLeft <= 0)
		{
			IsTimeLeft = false;
		}
		OnVictory();
	}
}

void ANetGameState::TriggerRestart_Implementation()
{
	TimeLeft = 30;
	OnRestart();
}

ANetPlayerState* ANetGameState::GetPlayerStateByIndex(int PlayerIndex)
{
	for (APlayerState* PS : PlayerArray)
	{
		ANetPlayerState* State = Cast<ANetPlayerState>(PS);
		if (State && State->PlayerIndex == PlayerIndex)
		{
			return State;
		}
	}

	return nullptr;
}

