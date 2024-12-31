// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetGameInstance.h"
#include "NetPlayerState.generated.h"

/**
 *
 */
UCLASS()
class ANetPlayerState : public APlayerState
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerInfo)
    FSPlayerInfo Data;

    UPROPERTY(BlueprintReadOnly, Replicated)
    int PlayerIndex;

    EPlayerTeam TeamID;
    EGameResults Result;

private:

    UFUNCTION()
    void OnRep_PlayerInfo();
};