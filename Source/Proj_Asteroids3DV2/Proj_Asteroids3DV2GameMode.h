// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Components/TimelineComponent.h"
#include "Proj_Asteroids3DV2GameMode.generated.h"

UCLASS(MinimalAPI)
class AProj_Asteroids3DV2GameMode : public AGameModeBase
{
	GENERATED_BODY()

	// How many enemies to spawn -- and how many needs to be killed before next stage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemies | AI", meta = (AllowPrivateAccess = "true"))
	int TotalEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> ProgressionPortalActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression", meta = (AllowPrivateAccess = "true"))
	float ProgressionPortalSpawnRadius;

	UPROPERTY(BlueprintReadOnly, Category = "Enemies | AI", meta = (AllowPrivateAccess = "true"))
	int CurrentEnemyCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
	TArray<class AActorSpawner *> Spawners;

	UPROPERTY(BlueprintReadOnly, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float ImageFadeAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeTime;

	UPROPERTY(BlueprintReadOnly, Category = "Progression", meta = (AllowPrivateAccess = "true"))
	int GalaxiesCleared;

	bool bDoFade;

	bool bFadeIn;

	class AActor *PortalActor;

protected:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	void SpawnProgressionPortal();

	FVector GetRandomPositionAtRadius(FVector Origin, float Radius);
	FVector GetRandomDirectionNearOrigin(FVector Origin, float DirectionToOriginRadius, FVector SpawnPos);

	UFUNCTION(BlueprintCallable)
	void InverseTeleport(FVector Origin, AActor *ActorToTeleport);

public:
	AProj_Asteroids3DV2GameMode();

	void AddDeadEnemyCount()
	{
		CurrentEnemyCount--;
		if (CurrentEnemyCount == 0)
			SpawnProgressionPortal();
	}

	bool AllEnemyAIDead() { return CurrentEnemyCount <= 0; }

	void ResetEnemyCount() { CurrentEnemyCount = TotalEnemies; }

	UFUNCTION(BlueprintCallable)
	void ProgressLevel();

	UFUNCTION(BlueprintCallable)
	void SetupFadeTransition(bool bFadeIn, float FadeDuration, float ImageStartAlpha = -1.f);
	void DoFadeTransition();
};
