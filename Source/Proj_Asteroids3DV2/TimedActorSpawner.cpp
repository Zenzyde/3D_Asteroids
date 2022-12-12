// Fill out your copyright notice in the Description page of Project Settings.

#include "TimedActorSpawner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATimedActorSpawner::ATimedActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnDelay = .7f;
}

// Called when the game starts or when spawned
void ATimedActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	ResetSpawnActorsOnRepeat();
}

void ATimedActorSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
