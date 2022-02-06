// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidSpawnerActor.h"
#include "PoissonDiscSampler.h"
#include "DrawDebugHelpers.h"
#include "AsteroidActor.h"
#include "Kismet/GameplayStatics.h"
#include "MiniProj_AsteroidsPawn.h"

// Sets default values
AAsteroidSpawnerActor::AAsteroidSpawnerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAsteroidSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	// TArray<FVector> positions = PoissonDiscSampler::SamplePoissonDiscPositions(sampleRegionSize, cellRadius);
	// if (positions.Num() > 0)
	// {
	// 	for (size_t i = 0; i < positions.Num(); i++)
	// 	{
	// 		FVector spawnPos = GetActorLocation() + FVector(sampleRegionOffset.Z, sampleRegionOffset.X + positions[i].X, sampleRegionOffset.Y + positions[i].Y);
	// 	}
	// }
}

// Called every frame
void AAsteroidSpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TimeSeconds > nextSpawn)
	{
		int max = FMath::RandRange(3, 6);
		for (size_t i = 0; i < max; i++)
		{
			if (!asteroidQueue.IsEmpty())
			{
				FVector spawnPos;
				asteroidQueue.Dequeue(OUT spawnPos);
				GetWorld()->SpawnActor<AAsteroidActor>(actorToSpawn, spawnPos, FRotator::ZeroRotator);
			}
		}

		if (asteroidQueue.IsEmpty())
		{
			TArray<FVector> positions = PoissonDiscSampler::SamplePoissonDiscPositions(sampleRegionSize, cellRadius);
			if (positions.Num() > 0)
			{
				for (size_t i = 0; i < positions.Num(); i++)
				{
					FVector spawnPos = GetActorLocation() + FVector(sampleRegionOffset.Z, sampleRegionOffset.X + positions[i].X, sampleRegionOffset.Y + positions[i].Y);
					asteroidQueue.Enqueue(spawnPos);
				}
			}
		}

		nextSpawn = GetWorld()->TimeSeconds + 2.f;
	}
}
