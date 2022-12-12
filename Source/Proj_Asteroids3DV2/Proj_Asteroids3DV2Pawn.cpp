// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proj_Asteroids3DV2Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Proj_Asteroids3DV2/Proj_Asteroids3DV2Bullet.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Proj_Asteroids3DV2/Proj_Asteroids3DV2GameMode.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "CharacterStateComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "EnemyAIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DelayForSeconds.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorSpawner.h"

AProj_Asteroids3DV2Pawn::AProj_Asteroids3DV2Pawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get()); // Set static mesh
	RootComponent = PlaneMesh;

	// Create static mesh component
	PlaneShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane Shield Mesh"));
	PlaneShieldMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get()); // Set static mesh
	PlaneShieldMesh->SetupAttachment(RootComponent);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent); // Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f;	   // The camera follows at this distance behind the character
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = false; // Do not allow camera to lag
	// SpringArm->CameraLagSpeed = 15.f;

	// Create camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera
	Camera->bUsePawnControlRotation = false;							 // Don't rotate camera with controller

	// Create AI Perception stimuli source
	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Perception Source"));

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 0.f; // 500.f;
	bCanTurn = true;
	MoveSpeed = 1000.f;

	HealthComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("Health Component"));

	ObjectiveDirectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Objective Direction Mesh"));
	ObjectiveDirectionMesh->SetupAttachment(RootComponent);
}

void AProj_Asteroids3DV2Pawn::BeginPlay()
{
	Super::BeginPlay();

	PlaneMesh->OnComponentBeginOverlap.AddDynamic(this, &AProj_Asteroids3DV2Pawn::OnComponentBeginOverlap);

	FLatentActionInfo info;
	info.CallbackTarget = this;
	info.Linkage = 1;
	info.UUID = 0;
	info.ExecutionFunction = FName("DelayedInitialization");
	GetWorld()->GetLatentActionManager().AddNewAction(this, 0, new FDelayForSeconds(0.1f, info));

	ObjectiveDirectionMesh->SetVisibility(true);
}

void AProj_Asteroids3DV2Pawn::DelayedInitialization()
{
	TArray<AActor *> ResultActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Asteroid"), ResultActors);
	for (AActor *Actor : ResultActors)
	{
		float Distance = FVector::Distance(GetActorLocation(), Actor->GetActorLocation());
		if (Distance > 2000.f)
			continue;

		Actor->Destroy();
	}
}

void AProj_Asteroids3DV2Pawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, CurrentRightSpeed * DeltaSeconds, CurrentUpSpeed * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame for root component
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate root component
	AddActorLocalRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	// Update fire rate time for widget
	CurrentFireRateTime += DeltaSeconds;

	// Clamp current fire rate time
	CurrentFireRateTime = FMath::Clamp(CurrentFireRateTime, 0.f, FireRate);

	// Update thrust input
	HandleThrustInput(DeltaSeconds);

	if (InvulnerabilitySpawnTimer > 0.0f)
		InvulnerabilitySpawnTimer -= DeltaSeconds;

	AProj_Asteroids3DV2GameMode *GameMode = Cast<AProj_Asteroids3DV2GameMode>(GetWorld()->GetAuthGameMode());

	if (!GameMode->AllEnemyAIDead())
	{
		TArray<AActor *> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIBaseCharacter::StaticClass(), Actors);

		float ClosestDist = FLT_MAX;
		AActor *ClosestActor = nullptr;

		float DotResult = 0;

		for (AActor *Actor : Actors)
		{
			float CurrentDistance = FVector::Distance(Actor->GetActorLocation(), GetActorLocation());
			if (CurrentDistance < ClosestDist)
			{
				ClosestDist = CurrentDistance;
				ClosestActor = Actor;

				DotResult = FVector::DotProduct((Actor->GetActorLocation() - GetActorLocation()).GetSafeNormal(), GetActorForwardVector());
			}
		}

		if (DotResult > .95f)
		{
			ObjectiveDirectionMesh->SetVisibility(false);
		}
		else
		{
			ObjectiveDirectionMesh->SetVisibility(true);

			FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(ObjectiveDirectionMesh->GetComponentLocation(), ClosestActor->GetActorLocation());

			ObjectiveDirectionMesh->SetWorldRotation(LookAt);
		}
	}
	else if (GameMode->AllEnemyAIDead() && !Portal)
	{
		TArray<AActor *> Actors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Portal"), Actors);

		Portal = Actors[0];
		ObjectiveDirectionMesh->SetVisibility(true);
	}
	else if (GameMode->AllEnemyAIDead() && Portal)
	{
		float DotResult = FVector::DotProduct((Portal->GetActorLocation() - GetActorLocation()).GetSafeNormal(), GetActorForwardVector());

		if (DotResult > .95f)
		{
			ObjectiveDirectionMesh->SetVisibility(false);
		}
		else
		{
			ObjectiveDirectionMesh->SetVisibility(true);

			FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(ObjectiveDirectionMesh->GetComponentLocation(), Portal->GetActorLocation());

			ObjectiveDirectionMesh->SetWorldRotation(LookAt);
		}
	}
}

void AProj_Asteroids3DV2Pawn::NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	// Not doing this anymore, a hit by an asteroid equals a loss, destroy the ship

	if (InvulnerabilitySpawnTimer > 0.0f)
		return;

	if (Cast<AProj_Asteroids3DV2Bullet>(Other))
		return;

	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(30);

		// Call death event if health is low enough
		if (HealthComponent->IsDead())
		{
			Destroy();
		}
	}

	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.25f)); // 0.025f));
}

void AProj_Asteroids3DV2Pawn::OnComponentBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &HitResult)
{
	// if (OtherActor && OtherActor->ActorHasTag(FName("Portal")))
	// {
	// 	AProj_Asteroids3DV2GameMode *GameMode = Cast<AProj_Asteroids3DV2GameMode>(GetWorld()->GetAuthGameMode());
	// 	GameMode->ProgressLevel();
	// }
}

float AProj_Asteroids3DV2Pawn::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCause)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCause);

	if (HealthComponent)
	{
		HealthComponent->UpdateCharacterState(DamageAmount, DamageEvent, EventInstigator, DamageCause);
		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s was damaged & has %f HP currently"), *GetOwner()->GetName(), HealthComponent->CurrentHealth));

		// Call death event if health is low enough
		if (HealthComponent->IsDead())
		{
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s has died!"), *GetOwner()->GetName()));
			Destroy();
		}

		return DamageAmount;
	}

	return 0;
}

void AProj_Asteroids3DV2Pawn::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("MoveUp", this, &AProj_Asteroids3DV2Pawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProj_Asteroids3DV2Pawn::MoveRightInput);
	PlayerInputComponent->BindAxis("RotateUp", this, &AProj_Asteroids3DV2Pawn::RotateUpInput);
	PlayerInputComponent->BindAxis("RotateRight", this, &AProj_Asteroids3DV2Pawn::RotateRightInput);

	// Bind action events
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProj_Asteroids3DV2Pawn::SpawnProjectile);
	PlayerInputComponent->BindAction("Thrust", IE_Pressed, this, &AProj_Asteroids3DV2Pawn::ActivateThrust);
	PlayerInputComponent->BindAction("Thrust", IE_Released, this, &AProj_Asteroids3DV2Pawn::DeactivateThrust);
	PlayerInputComponent->BindAction("Exit", IE_Pressed, this, &AProj_Asteroids3DV2Pawn::ExitGame);
}

void AProj_Asteroids3DV2Pawn::ActivateThrust()
{
	// Set thrusting flag
	bIsThrusting = true;
}

void AProj_Asteroids3DV2Pawn::DeactivateThrust()
{
	// Unset thrusting flag
	bIsThrusting = false;
}

void AProj_Asteroids3DV2Pawn::HandleThrustInput(float DeltaSeconds)
{
	// If we are thrusting
	if (bIsThrusting)
	{
		// Count down the amount we have left for thrust
		CurrentThrustAmount -= ThrustDepletionRate * DeltaSeconds;
		// And if thrust amount reaches zero, unset thrust flag
		if (CurrentThrustAmount <= 0.f)
			DeactivateThrust();
	}
	else
	{
		// If we are not thrusting, add to thrust amount
		CurrentThrustAmount += ThrustAccumulationRate * DeltaSeconds;
	}
	// Clamp thrust amount between 0 and max thrust
	CurrentThrustAmount = FMath::Clamp(CurrentThrustAmount, 0.f, MaxThrustAmount);

	// Set interpolation value based on thrusting input
	float Val = bIsThrusting && CurrentThrustAmount > 0.f ? 1.f : 0.f;
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.75f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (DeltaSeconds * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, 0.f, MaxSpeed); // MinSpeed, MaxSpeed);

	// Calculate current FOV based on current speed
	// Subtract min to bring it to 0-1 range when going from 0 to max
	float T = ((CurrentForwardSpeed - MinSpeed) / (MaxSpeed - MinSpeed));
	// Calculate FOV using linear interpolation formula between 90 and 120 FOV
	// (made slight error, didn't need base + float-adjustment at start, made it overshoot) --
	// 1: A + (1f - T) * A + B * T
	// 2: (1f - T) * A + B * T
	float FOV = ((1.f - T) * 90.f) + (145.f * T);

	// Set camera FOV to match current forward speed
	Camera->SetFieldOfView(FOV);
}

void AProj_Asteroids3DV2Pawn::MoveUpInput(float Val)
{
	// Make Val based on bCanTurn boolean -- false -> 0, true -> 1
	Val *= bCanTurn;

	// Target pitch speed is based in input
	float TargetUpSpeed = (Val * MoveSpeed);

	// When steering, we decrease pitch slightly
	// TargetUpSpeed += (FMath::Abs(CurrentRightSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentUpSpeed = FMath::FInterpTo(CurrentUpSpeed, TargetUpSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AProj_Asteroids3DV2Pawn::MoveRightInput(float Val)
{
	// Make Val based on bCanTurn boolean -- false -> 0, true -> 1
	Val *= bCanTurn;

	// Target yaw speed is based on input
	float TargetRightSpeed = (Val * MoveSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentRightSpeed = FMath::FInterpTo(CurrentRightSpeed, TargetRightSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AProj_Asteroids3DV2Pawn::RotateUpInput(float Val)
{
	// Make Val based on bCanTurn boolean -- false -> 0, true -> 1
	Val *= bCanTurn;

	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AProj_Asteroids3DV2Pawn::RotateRightInput(float Val)
{
	// Make Val based on bCanTurn boolean -- false -> 0, true -> 1
	Val *= bCanTurn;

	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	float TargetRollSpeed = CurrentYawSpeed * 0.5f; // bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AProj_Asteroids3DV2Pawn::SpawnProjectile()
{
	// Exit out if we just fired a shot
	if (CurrentFireRateTime < FireRate)
		return;

	// Calculate spawn position and rotation
	const FVector SpawnPos(GetActorLocation() + GetActorForwardVector() * ProjectileSpawnOffset.X);

	const FRotator SpawnRot(GetActorForwardVector().ToOrientationRotator());

	// Spawn parameters
	const FTransform SpawnTransform(SpawnRot, SpawnPos);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn projectile
	UWorld *world = GetWorld();
	if (world)
	{
		world->SpawnActor<AProj_Asteroids3DV2Bullet>(Bullet, SpawnTransform, SpawnParams);
		CurrentFireRateTime = 0.f;
	}
}

float AProj_Asteroids3DV2Pawn::GetMaxSpeed() { return MaxSpeed; }
float AProj_Asteroids3DV2Pawn::GetCurrentForwardSpeed() { return CurrentForwardSpeed; }

void AProj_Asteroids3DV2Pawn::ExitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void AProj_Asteroids3DV2Pawn::SetTurnState(bool state)
{
	bCanTurn = state;
}