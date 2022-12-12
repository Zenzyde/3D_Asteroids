// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine.h"

// Sets default values
AActorSpawner::AActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AActorSpawner::GetSpawnPosition(float MinRadius, float MaxRadius, FName ActorTagToIgnore, bool bCheckForOverlap)
{
	const FVector Origin = GetActorLocation();

	const FVector Offset = UKismetMathLibrary::RandomUnitVector() * UKismetMathLibrary::RandomFloatInRange(MinRadius, MaxRadius);

	if (!bCheckForOverlap)
		return Origin + Offset;

	int MaxAttempts = 100;
	int CurrentAttempt = 0;

	const UWorld *World = GetWorld();

	const FVector Extents = FVector::OneVector * 5.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> IgnoreObjectTypes;
	IgnoreObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	IgnoreObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	IgnoreObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor *> ActorsToIgnore;
	UGameplayStatics::GetAllActorsWithTag(World, ActorTagToIgnore, ActorsToIgnore);

	while (CurrentAttempt < MaxAttempts)
	{
		FVector SpawnPos = Origin + Offset;

		TArray<AActor *> ResultActors;
		if (!UKismetSystemLibrary::BoxOverlapActors(GetWorld(), SpawnPos, Extents, IgnoreObjectTypes, NULL, ActorsToIgnore, ResultActors))
		{
			return SpawnPos;
		}

		CurrentAttempt++;
	}

	return FVector::ZeroVector;
}

FVector AActorSpawner::GetRandomDirectionCloseToCenter(FVector SpawnPosition, float RadiusFromCenter)
{
	const FVector Origin = GetActorLocation();

	const FVector Offset = UKismetMathLibrary::RandomUnitVector() * (RadiusFromCenter / 2.f);

	const FVector Center = Origin + Offset;

	const FVector Direction = (Center - SpawnPosition).GetSafeNormal();

	return Direction;
}

void AActorSpawner::SpawnActors()
{
	int NumActors = UKismetMathLibrary::RandomIntegerInRange(MinActorAmount, MaxActorAmount);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (bDoOverlapCheck)
	{
		for (size_t i = 0; i < NumActors; i++)
		{
			const FVector SpawnPos = GetSpawnPosition(MinimumRadius, MaximumRadius, IgnoreActorTag, true);

			const FVector Direction = UKismetMathLibrary::RandomUnitVector();

			AActor *Asteroid = GetWorld()->SpawnActor<AActor>(SpawnableActor, SpawnPos, Direction.ToOrientationRotator(), SpawnParams);
		}
	}
	else
	{
		for (size_t i = 0; i < NumActors; i++)
		{
			const FVector SpawnPos = GetSpawnPosition(MinimumRadius, MaximumRadius, IgnoreActorTag, false);

			const FVector Direction = GetRandomDirectionCloseToCenter(SpawnPos, CenterRadius);

			AActor *Asteroid = GetWorld()->SpawnActor<AActor>(SpawnableActor, SpawnPos, Direction.ToOrientationRotator(), SpawnParams);
		}
	}
}

void AActorSpawner::SpawnActorsWithParams(int MinNumActors, int MaxNumActors, TSubclassOf<class AActor> ActorToSpawn, float MinRadius, float MaxRadius, float RadiusFromCenter, FName ActorTagToIgnore, bool bCheckForOverlap)
{
	int NumActors = UKismetMathLibrary::RandomIntegerInRange(MinNumActors, MaxNumActors);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (bCheckForOverlap)
	{
		for (size_t i = 0; i < NumActors; i++)
		{
			const FVector SpawnPos = GetSpawnPosition(MinRadius, MaxRadius, ActorTagToIgnore, true);

			const FVector Direction = UKismetMathLibrary::RandomUnitVector();

			AActor *Asteroid = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnPos, Direction.ToOrientationRotator(), SpawnParams);
		}
	}
	else
	{
		for (size_t i = 0; i < NumActors; i++)
		{
			const FVector SpawnPos = GetSpawnPosition(MinRadius, MaxRadius, ActorTagToIgnore, false);

			const FVector Direction = GetRandomDirectionCloseToCenter(SpawnPos, RadiusFromCenter);

			AActor *Asteroid = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnPos, Direction.ToOrientationRotator(), SpawnParams);
		}
	}
}

void AActorSpawner::DeleteActors()
{
	SpawnedActors.Empty();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnableActor->StaticClass(), SpawnedActors);

	for (AActor *Actor : SpawnedActors)
	{
		if (Actor != NULL)
			Actor->Destroy();
	}

	SpawnedActors.Empty();
}

bool AActorSpawner::GetDoesOverlapCheck()
{
	return bDoOverlapCheck;
}
