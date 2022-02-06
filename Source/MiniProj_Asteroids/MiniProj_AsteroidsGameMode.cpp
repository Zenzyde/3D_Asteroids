// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniProj_AsteroidsGameMode.h"
#include "MiniProj_AsteroidsPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Particles/ParticleSystem.h"

AMiniProj_AsteroidsGameMode::AMiniProj_AsteroidsGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AMiniProj_AsteroidsPawn::StaticClass();

	IsPlayerDead = false;
}

void AMiniProj_AsteroidsGameMode::ReloadGame()
{
	UWorld *world = GetWorld();
	UGameplayStatics::OpenLevel(world, FName(world->GetName()), false);
}

void AMiniProj_AsteroidsGameMode::QuitGame()
{
	UWorld *world = GetWorld();
	APlayerController *controller = Cast<APlayerController>(world->GetFirstPlayerController());
	UKismetSystemLibrary::QuitGame(GetWorld(), controller, EQuitPreference::Quit, false);
}

void AMiniProj_AsteroidsGameMode::SpawnVFX(UParticleSystem *particle, FVector position)
{
	UWorld *world = GetWorld();
	FTransform spawnTransform;
	spawnTransform.SetLocation(position);
	UGameplayStatics::SpawnEmitterAtLocation(world, particle, spawnTransform);
}