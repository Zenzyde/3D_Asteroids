// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniProj_AsteroidsGameMode.generated.h"

UCLASS(MinimalAPI)
class AMiniProj_AsteroidsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMiniProj_AsteroidsGameMode();

	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerDead;

	UFUNCTION(BlueprintCallable)
	void ReloadGame();

	UFUNCTION(BlueprintCallable)
	void QuitGame();

	UFUNCTION(BlueprintCallable)
	void SpawnVFX(class UParticleSystem *particle, FVector position);
};
