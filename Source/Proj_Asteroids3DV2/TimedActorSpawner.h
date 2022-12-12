// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorSpawner.h"
#include "TimedActorSpawner.generated.h"

/**
 *
 */
UCLASS()
class PROJ_ASTEROIDS3DV2_API ATimedActorSpawner : public AActorSpawner
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float SpawnDelay;

	float NextSpawnTime;

	FTimerHandle RepeatSpawnHandle;

public:
	ATimedActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void DeleteActors() override
	{
		if (GetWorldTimerManager().IsTimerActive(RepeatSpawnHandle))
			GetWorldTimerManager().ClearTimer(RepeatSpawnHandle);

		Super::DeleteActors();
	};

	void ResetSpawnActorsOnRepeat()
	{
		GetWorldTimerManager().SetTimer(RepeatSpawnHandle, this, &AActorSpawner::SpawnActors, SpawnDelay, true, 0.5f);
	};
};
