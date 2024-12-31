// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetBaseCharacter.h"
#include "Net/UnrealNetwork.h"

void ANetPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANetPlayerState, Data);
    DOREPLIFETIME(ANetPlayerState, PlayerIndex);
}

void ANetPlayerState::OnRep_PlayerInfo()
{
    ANetBaseCharacter* Char = Cast<ANetBaseCharacter>(GetPawn());
    if (Char)
    {
        Char->PlayerInfoReceived = true;
    }
    else
    {
        FTimerHandle PlayerInfoUpdateTimer;
        GWorld->GetTimerManager().SetTimer(PlayerInfoUpdateTimer, this, &ANetPlayerState::OnRep_PlayerInfo, 0.25f, false);
    }
}