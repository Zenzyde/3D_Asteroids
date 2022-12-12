// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawner.generated.h"

UCLASS()
class PROJ_ASTEROIDS3DV2_API AActorSpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	int MinActorAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	int MaxActorAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MinimumRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MaximumRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float CenterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FName IgnoreActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	bool bDoOverlapCheck;

	TArray<AActor *> SpawnedActors;

public:
	// Sets default values for this actor's properties
	AActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetSpawnPosition(float MinRadius, float MaxRadius, FName ActorTagToIgnore, bool bCheckForOverlap = false);

	FVector GetRandomDirectionCloseToCenter(FVector SpawnPosition, float RadiusFromCenter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> SpawnableActor;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnActors();
	UFUNCTION(BlueprintCallable)
	void SpawnActorsWithParams(int MinNumActors, int MaxNumActors, TSubclassOf<class AActor> ActorToSpawn, float MinRadius, float MaxRadius, float RadiusFromCenter, FName ActorTagToIgnore, bool bCheckForOverlap = false);

	virtual void DeleteActors();

	bool GetDoesOverlapCheck();

	void SetMaximumActorAmount(int NewAmount) { MaxActorAmount = NewAmount; };
	void SetMinimumActorAmount(int NewAmount) { MinActorAmount = NewAmount; };
};
