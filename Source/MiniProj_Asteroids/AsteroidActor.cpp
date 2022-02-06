// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MiniProj_AsteroidsPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "MiniProj_AsteroidsGameMode.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
AAsteroidActor::AAsteroidActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// meant for collision detection primarily
	asteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Asteroid_Mesh"));
	RootComponent = asteroidMesh;

	// meant for visuals primarily
	asteroidVfx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Asteroid_VFX"));
	asteroidVfx->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAsteroidActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAsteroidActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalOffset(FVector::ForwardVector * DeltaTime * -moveSpeed, true);
}

void AAsteroidActor::NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	AMiniProj_AsteroidsPawn *player = Cast<AMiniProj_AsteroidsPawn>(Other);

	AProjectileActor *bullet = Cast<AProjectileActor>(Other);

	// Only spawn vfx if asteroid is destroyed by player in some way
	if (explosionVfx != nullptr && (player != nullptr || bullet != nullptr))
	{
		UWorld *world = GetWorld();
		AMiniProj_AsteroidsGameMode *gameMode = Cast<AMiniProj_AsteroidsGameMode>(world->GetAuthGameMode());
		if (!gameMode->IsPlayerDead)
		{
			gameMode->SpawnVFX(explosionVfx, HitLocation);
			if (camShake != nullptr)
			{
				UGameplayStatics::PlayWorldCameraShake(world, camShake, GetActorLocation(), 500.0f, 50000.0f, 1.0f);
			}
		}
	}

	Destroy();
}
