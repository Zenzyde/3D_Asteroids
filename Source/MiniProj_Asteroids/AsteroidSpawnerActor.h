// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidActor.h"
#include "MiniProj_AsteroidsPawn.h"
#include "AsteroidSpawnerActor.generated.h"

UCLASS()
class MINIPROJ_ASTEROIDS_API AAsteroidSpawnerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAsteroidSpawnerActor();

private:
	UPROPERTY(EditAnywhere)
	FVector sampleRegionSize;

	UPROPERTY(EditAnywhere)
	FVector sampleRegionOffset;

	UPROPERTY(EditAnywhere)
	float cellRadius;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidActor> actorToSpawn;

	float nextSpawn;

	TQueue<FVector> asteroidQueue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
