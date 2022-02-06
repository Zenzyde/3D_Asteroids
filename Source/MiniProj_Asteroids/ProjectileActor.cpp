// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "AsteroidActor.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile_Mesh"));
	RootComponent = projectileMesh;

	projectileVfx = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile_VFX"));
	projectileVfx->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalOffset(FVector::UpVector * DeltaTime * moveSpeed, true);
}

void AProjectileActor::NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	Destroy();
}