// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetAvatar.h"
#include "GameFramework/GameModeBase.h"
#include "NetGameMode.generated.h"

/**
 *
 */
UCLASS()
class ANetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetGameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION(BlueprintCallable)
	void AvatarsOverlapped(ANetAvatar* AvatarA, ANetAvatar* AvatarB);

	UFUNCTION(BlueprintCallable)
	void EndGame();

private:
	int TotalPlayerCount;
	int TotalGames;
	int PlayerStartIndex;

	TArray<APlayerController*> AllPlayers;


	AActor* GetPlayerStart(FString Name, int Index);

	AActor* AssignTeamAndPlayerStart(AController* Player);
};
