// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameState.generated.h"

/**
 *
 */
UCLASS()
class ANetGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANetGameState();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Winner)
	int WinningPlayer;

	UFUNCTION()
	void OnRep_Winner();

	UFUNCTION(BlueprintImplementableEvent)
	void OnVictory();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRestart();

	UFUNCTION(NetMulticast, Reliable)
	void TriggerRestart();

	UFUNCTION(BlueprintCallable)
	ANetPlayerState* GetPlayerStateByIndex(int PlayerIndex);
};

