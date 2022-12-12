// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proj_Asteroids3DV2GameMode.h"
#include "Proj_Asteroids3DV2Pawn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorSpawner.h"
#include "TimedActorSpawner.h"
#include "Components/TimelineComponent.h"
#include "Engine.h"

AProj_Asteroids3DV2GameMode::AProj_Asteroids3DV2GameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AProj_Asteroids3DV2Pawn::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void AProj_Asteroids3DV2GameMode::StartPlay()
{
	Super::StartPlay();

	SetupFadeTransition(true, 1.f, 1.f);

	CurrentEnemyCount = TotalEnemies;

	TArray<AActor *> ResultActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorSpawner::StaticClass(), ResultActors);
	for (size_t i = 0; i < ResultActors.Num(); i++)
	{
		Spawners.Add(Cast<AActorSpawner>(ResultActors[i]));
	}

	for (size_t i = 0; i < Spawners.Num(); i++)
	{
		if (Spawners[i]->IsValidLowLevel())
		{
			if (Spawners[i]->GetDoesOverlapCheck())
			{
				Spawners[i]->SetMaximumActorAmount(CurrentEnemyCount);
				Spawners[i]->SetMinimumActorAmount(CurrentEnemyCount);
			}
			Spawners[i]->SpawnActors();
		}
	}
}

void AProj_Asteroids3DV2GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDoFade)
	{
		DoFadeTransition();
	}
}

void AProj_Asteroids3DV2GameMode::SpawnProgressionPortal()
{
	if (!ProgressionPortalActor)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector Location = UKismetMathLibrary::RandomUnitVector() * (UKismetMathLibrary::RandomFloat() * ProgressionPortalSpawnRadius);
	PortalActor = GetWorld()->SpawnActor<AActor>(ProgressionPortalActor, Location, FRotator::ZeroRotator, SpawnParams);
}

FVector AProj_Asteroids3DV2GameMode::GetRandomPositionAtRadius(FVector Origin, float Radius)
{
	return Origin + UKismetMathLibrary::RandomUnitVector() * Radius;
}

FVector AProj_Asteroids3DV2GameMode::GetRandomDirectionNearOrigin(FVector Origin, float DirectionToOriginRadius, FVector SpawnPos)
{
	return ((Origin + UKismetMathLibrary::RandomUnitVector() * DirectionToOriginRadius) - SpawnPos).GetSafeNormal();
}

void AProj_Asteroids3DV2GameMode::InverseTeleport(FVector Origin, AActor *ActorToTeleport)
{
	const FVector NewLocation = (Origin - ActorToTeleport->GetActorLocation()) + Origin;

	ActorToTeleport->SetActorLocation(NewLocation);

	const FVector NewDirection = (Origin - ActorToTeleport->GetActorLocation()).GetSafeNormal();

	ActorToTeleport->SetActorRotation(NewDirection.ToOrientationRotator());
}

void AProj_Asteroids3DV2GameMode::ProgressLevel()
{
	GalaxiesCleared++;

	TotalEnemies += 4;
	CurrentEnemyCount = TotalEnemies;

	// Clear all asteroids and tell spawner to spawn new ones
	if (Spawners.Num() > 0)
	{
		Spawners[0]->DeleteActors();

		for (AActorSpawner *Spawner : Spawners)
		{
			checkf(Spawner != NULL, TEXT("Spawner null state: %s"), Spawner == NULL ? TEXT("NULL") : TEXT("NOT-NULL"));

			if (Spawner->GetDoesOverlapCheck())
			{
				Spawner->SetMaximumActorAmount(CurrentEnemyCount);
				Spawner->SetMinimumActorAmount(CurrentEnemyCount);
			}

			Spawner->SpawnActors();

			ATimedActorSpawner *TimedSpawner = Cast<ATimedActorSpawner>(Spawner);
			if (TimedSpawner)
				TimedSpawner->ResetSpawnActorsOnRepeat();
		}
	}

	// Destroy old portal
	if (PortalActor)
		PortalActor->Destroy();
}

void AProj_Asteroids3DV2GameMode::SetupFadeTransition(bool bIn, float FadeDuration, float ImageStartAlpha)
{
	if (ImageStartAlpha > -1.f)
		ImageFadeAmount = ImageStartAlpha;
	bFadeIn = bIn;
	FadeTime = FadeDuration;
	bDoFade = true;
}

void AProj_Asteroids3DV2GameMode::DoFadeTransition()
{
	ImageFadeAmount = FMath::FInterpTo(ImageFadeAmount, bFadeIn ? 0.f : 1.f, GetWorld()->GetDeltaSeconds(), FadeTime);

	bDoFade = !(bFadeIn ? FMath::IsNearlyZero(ImageFadeAmount) : FMath::IsNearlyEqual(ImageFadeAmount, 1.f));
}