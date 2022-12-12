// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "CharacterStateComponent.h"
#include "Engine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "NavigationPath.h"
#include "LatentActions.h"
#include "DelayForSeconds.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/LatentActionManager.h"
#include "Components/TimelineComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Proj_Asteroids3DV2/Proj_Asteroids3DV2Pawn.h"
#include "Proj_Asteroids3DV2/Proj_Asteroids3DV2GameMode.h"
#include "Proj_Asteroids3DV2/EnemyAIBaseController.h"
#include "Proj_Asteroids3DV2/Proj_Asteroids3DV2Bullet.h"

// Sets default values
AEnemyAIBaseCharacter::AEnemyAIBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup to enable smooth rotation to controller desired rotation for smooth ai rotation, instead of matching controller desired rotation 1-to-1 constantly
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Set movement mode
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
	GetCharacterMovement()->GravityScale = 0.f;
	BaseEyeHeight = 0.f;
	CrouchedEyeHeight = 0.f;
	bUseControllerRotationPitch = true;

	HealthComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("Health Component"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	AISightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Sense"));
	AIPerceptionComponent->ConfigureSense(*AISightSenseConfig);
	AIPerceptionComponent->SetDominantSense(AISightSenseConfig->GetSenseImplementation());

	// Create AI Perception stimuli source
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Perception Source"));

	DetectionAwarenessTimer = 10.f;
}

// Called when the game starts or when spawned
void AEnemyAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Assign AI Controller
	AIController = Cast<AEnemyAIBaseController>(GetController());

	FLatentActionInfo info;
	info.CallbackTarget = this;
	info.Linkage = 1;
	info.UUID = 0;
	info.ExecutionFunction = FName("InitializationAfterPossession");
	GetWorld()->GetLatentActionManager().AddNewAction(this, 0, new FDelayForSeconds(1.f, info));

	// Bind AI Sense to sense event
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIBaseCharacter::OnAISenseTargetUpdated);

	FleeThreshold = FMath::RandRange(20.f, 80.f);
}

void AEnemyAIBaseCharacter::InitializationAfterPossession()
{
	// if (AIController)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Controller is valid for %s"), *GetName()));
	// }

	if (AIController && AIController->GetBlackboardComponent())
	{
		BlackboardComponent = AIController->GetBlackboardComponent();

		// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Blackboard state: %s for %s"), BlackboardComponent ? TEXT("true") : TEXT("false"), *GetName()));

		if (BlackboardComponent)
		{
			AIController->UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent);
			BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Blackboard setup completed!")));
		}
	}
}

// Called every frame
void AEnemyAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DetectionAwarenessTimer > 0.f)
	{
		DetectionAwarenessTimer -= DeltaTime;
		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Time: %f"), DetectionAwarenessTimer));
	}

	VerifyAISensedActors();
}

void AEnemyAIBaseCharacter::OnAISenseTargetUpdated(AActor *SensedActor, FAIStimulus AIStimulus)
{
	if (DetectionAwarenessTimer > 0.f)
		return;

	AProj_Asteroids3DV2Pawn *PlayerCharacter = Cast<AProj_Asteroids3DV2Pawn>(SensedActor);
	if (PlayerCharacter)
		Player = AIStimulus.WasSuccessfullySensed() ? PlayerCharacter : NULL;
}

void AEnemyAIBaseCharacter::VerifyAISensedActors()
{
	// if (Player)
	// {
	// 	FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	// 	float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToPlayer, GetActorForwardVector())));
	// 	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 500.f, 15.f, FColor::Yellow);

	// 	if (Angle <= DetectionFOV)
	// 		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), Player->GetActorLocation(), 15.f, FColor::Green);
	// 	else
	// 		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), Player->GetActorLocation(), 15.f, FColor::Red);
	// }

	if (BlackboardComponent)
	{
		if (Player)
		{
			BlackboardComponent->SetValueAsBool(FName("PlayerSpotted"), true);

			// FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			// float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToPlayer, GetActorForwardVector())));

			// if (Angle <= DetectionFOV)
			// {
			// 	BlackboardComponent->SetValueAsBool(FName("PlayerSpotted"), true);
			// 	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Blackboard key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("PlayerSpotted")) ? TEXT("true") : TEXT("false")));

			// 	if (HealthComponent->CurrentHealth <= HealthComponent->MaxHealth / 2.f)
			// 	{
			// 		BlackboardComponent->SetValueAsBool(FName("Flee"), true);
			// 		// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Blackboard Flee key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("Flee")) ? TEXT("true") : TEXT("false")));
			// 	}
			// }
			// else if (Angle > DetectionFOV)
			// {
			// 	BlackboardComponent->SetValueAsBool(FName("PlayerSpotted"), false);
			// 	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Blackboard key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("PlayerSpotted")) ? TEXT("true") : TEXT("false")));

			// 	if (HealthComponent->CurrentHealth <= HealthComponent->MaxHealth / 2.f)
			// 	{
			// 		BlackboardComponent->SetValueAsBool(FName("Flee"), false);
			// 		// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Blackboard Flee key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("Flee")) ? TEXT("true") : TEXT("false")));
			// 	}
			// }
		}
		else if (!Player)
		{
			BlackboardComponent->SetValueAsBool(FName("PlayerSpotted"), false);
			// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Blackboard key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("PlayerSpotted")) ? TEXT("true") : TEXT("false")));

			// if (HealthComponent->CurrentHealth <= HealthComponent->MaxHealth / 2.f)
			// {
			// 	BlackboardComponent->SetValueAsBool(FName("Flee"), false);
			// 	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Blackboard Flee key set: %s : %s"), Player ? TEXT("true") : TEXT("false"), BlackboardComponent->GetValueAsBool(FName("Flee")) ? TEXT("true") : TEXT("false")));
			// }
		}
	}
}

float AEnemyAIBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCause)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCause);

	if (!AIController)
		return 0;

	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(FName("PlayerSpotted"), true);
	}

	if (HealthComponent)
	{
		HealthComponent->UpdateCharacterState(DamageAmount, DamageEvent, EventInstigator, DamageCause);
		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s was damaged & has %f HP currently"), *GetOwner()->GetName(), HealthComponent->CurrentHealth));

		// Call death event if health is low enough
		if (HealthComponent->IsDead())
		{
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s has died!"), *GetOwner()->GetName()));
			OnDeath();
		}
		else if (HealthComponent->CurrentHealth <= FleeThreshold)
		{
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValueAsBool(FName("Flee"), true);
			}
		}

		return DamageAmount;
	}

	return 0;
}

void AEnemyAIBaseCharacter::OnDeath()
{
	// Make sure enemy capsule can't be collided with when ragdolling
	if (GetCapsuleComponent() != nullptr)
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Effects

	// Stop AI execution
	if (AIController != nullptr)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s is an AI and was unpossessed by it's controller"), *GetOwner()->GetName()));
		AIController->UnPossess();
		AIController->Destroy();
	}

	// Update current enemy count in gamemode
	AProj_Asteroids3DV2GameMode *GameMode = Cast<AProj_Asteroids3DV2GameMode>(GetWorld()->GetAuthGameMode());
	GameMode->AddDeadEnemyCount();

	Destroy();
}

bool AEnemyAIBaseCharacter::SpawnProjectile(class UClass *Projectile, FVector Position, FVector Direction, float Speed)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	AProj_Asteroids3DV2Bullet *ProjectileActor = GetWorld()->SpawnActor<AProj_Asteroids3DV2Bullet>(Projectile, Position, Direction.ToOrientationRotator(), SpawnParams);

	return ProjectileActor != NULL;
}

bool AEnemyAIBaseCharacter::TryMoveToRandomAirLocation(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance)
{
	if (!AIController)
		return false;

	if (TargetLocation == FVector::ZeroVector)
	{
		FVector Origin = GetActorLocation();
		FVector RandDirection = UKismetMathLibrary::RandomUnitVector();
		RandDirection.Normalize();
		TargetLocation = Origin + RandDirection * (Radius * UKismetMathLibrary::RandomFloat());

		// Confirm location is reachable using linetracing
		TArray<AActor *> actorsToIgnore;
		actorsToIgnore.Add(this);
		FHitResult hitResult;

		if (UKismetSystemLibrary::SphereTraceSingleByProfile(
				GetWorld(), GetActorLocation(), TargetLocation, SpheretraceRadius, FName(UCollisionProfile::BlockAll_ProfileName), false, actorsToIgnore,
				EDrawDebugTrace::None, hitResult, true, FLinearColor::Red, FLinearColor::Green, 1.5f))
		{
			TargetLocation = FVector::ZeroVector;
			return false;
		}
	}

	AIController->SetFocalPoint(TargetLocation);

	float DistanceToTarget = FVector::Distance(TargetLocation, GetActorLocation());

	if (DistanceToTarget <= ArriveDistance)
	{
		GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
		TargetLocation = FVector::ZeroVector;
		return true;
	}
	else
	{
		// Check for other AI that might be on collision course
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		FHitResult hitResult;
		FCollisionShape shape;
		shape.SetSphere(SpheretraceRadius);
		if (GetWorld()->SweepSingleByChannel(hitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * LookForCollisionDistance, FQuat::Identity, ECollisionChannel::ECC_Visibility, shape, CollisionQueryParams))
		{
			GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
			TargetLocation = FVector::ZeroVector;
			return false;
		}

		GetCharacterMovement()->BrakingDecelerationFlying = 0.f;
		GetCharacterMovement()->Velocity = GetActorForwardVector() * FlySpeed;

		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), TargetLocation, 25.f, FColor::Green, false, 1.f, 15.f);

		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), TargetLocation, 25.f, FColor::Green, false, 1.f, 15.f);
		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), AIController->GetFocalPoint(), 25.f, FColor::Blue, false, 1.f, 15.f);
		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, 25.f, FColor::Red, false, 1.f, 15.f);
		return false;
	}
}

void AEnemyAIBaseCharacter::AttackPlayer(UClass *Projectile, float Speed, float SpawnOffset)
{
	if (!AIController || !Player)
		return;

	AIController->SetFocalPoint(Player->GetActorLocation());

	GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;

	SpawnProjectile(Projectile, GetActorLocation() + GetActorForwardVector() * SpawnOffset, GetActorForwardVector(), Speed);
}

bool AEnemyAIBaseCharacter::EvadePlayer(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance)
{
	if (!AIController || !Player)
		return false;

	if (TargetLocation == FVector::ZeroVector)
	{
		FVector Origin = GetActorLocation();
		FVector RandDirection = UKismetMathLibrary::RandomUnitVector();
		TargetLocation = Origin + RandDirection * (Radius * UKismetMathLibrary::RandomFloat());

		FVector ToTarget = (TargetLocation - Origin).GetSafeNormal();
		FVector ToPlayer = (Player->GetActorLocation() - Origin).GetSafeNormal();

		while (FMath::Abs(FVector::DotProduct(ToTarget, ToPlayer)) > 0.25f)
		{
			RandDirection = UKismetMathLibrary::RandomUnitVector();
			TargetLocation = Origin + RandDirection * (Radius * UKismetMathLibrary::RandomFloat());

			ToTarget = (TargetLocation - Origin).GetSafeNormal();
			ToPlayer = (Player->GetActorLocation() - Origin).GetSafeNormal();
		}

		// Confirm location is reachable using linetracing
		TArray<AActor *> actorsToIgnore;
		actorsToIgnore.Add(this);
		FHitResult hitResult;

		if (UKismetSystemLibrary::SphereTraceSingleByProfile(
				GetWorld(), GetActorLocation(), TargetLocation, SpheretraceRadius, FName(UCollisionProfile::BlockAll_ProfileName), false, actorsToIgnore,
				EDrawDebugTrace::None, hitResult, true, FLinearColor::Red, FLinearColor::Green, 1.5f))
		{
			TargetLocation = FVector::ZeroVector;
			return false;
		}
	}

	AIController->SetFocalPoint(Player->GetActorLocation());

	float DistanceToTarget = FVector::Distance(TargetLocation, GetActorLocation());

	if (DistanceToTarget <= ArriveDistance)
	{
		GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
		TargetLocation = FVector::ZeroVector;
		return true;
	}
	else
	{
		FVector ToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();

		// Check for other AI that might be on collision course
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		FHitResult hitResult;
		FCollisionShape shape;
		shape.SetSphere(SpheretraceRadius);
		if (GetWorld()->SweepSingleByChannel(hitResult, GetActorLocation(), GetActorLocation() + ToTarget * LookForCollisionDistance, FQuat::Identity, ECollisionChannel::ECC_Visibility, shape, CollisionQueryParams))
		{
			GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
			TargetLocation = FVector::ZeroVector;
			return false;
		}

		GetCharacterMovement()->BrakingDecelerationFlying = 0.f;
		GetCharacterMovement()->Velocity = ToTarget * FlySpeed;

		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), TargetLocation, 25.f, FColor::Yellow, false, 1.f, 15.f);

		return false;
	}
}

bool AEnemyAIBaseCharacter::EscapePlayer(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance)
{
	if (!AIController || !Player)
		return false;

	GetCharacterMovement()->BrakingDecelerationFlying = 0.f;

	if (TargetLocation == FVector::ZeroVector)
	{
		FVector Origin = GetActorLocation();
		FVector RandDirection = UKismetMathLibrary::RandomUnitVector();
		TargetLocation = Origin + RandDirection * (Radius * UKismetMathLibrary::RandomFloat());

		FVector ToTarget = (TargetLocation - Origin).GetSafeNormal();
		FVector ToPlayer = (Player->GetActorLocation() - Origin).GetSafeNormal();

		while (FVector::DotProduct(ToTarget, ToPlayer) > -0.7f)
		{
			RandDirection = UKismetMathLibrary::RandomUnitVector();
			TargetLocation = Origin + RandDirection * (Radius * UKismetMathLibrary::RandomFloat());

			ToTarget = (TargetLocation - Origin).GetSafeNormal();
			ToPlayer = (Player->GetActorLocation() - Origin).GetSafeNormal();
		}

		// Confirm location is reachable using linetracing
		TArray<AActor *> actorsToIgnore;
		actorsToIgnore.Add(this);
		FHitResult hitResult;

		if (UKismetSystemLibrary::SphereTraceSingleByProfile(
				GetWorld(), GetActorLocation(), TargetLocation, SpheretraceRadius, FName(UCollisionProfile::BlockAll_ProfileName), false, actorsToIgnore,
				EDrawDebugTrace::None, hitResult, true, FLinearColor::Red, FLinearColor::Green, 1.5f))
		{
			TargetLocation = FVector::ZeroVector;
			return false;
		}
	}

	AIController->SetFocalPoint(TargetLocation);

	float DistanceToTarget = FVector::Distance(TargetLocation, GetActorLocation());

	if (DistanceToTarget <= ArriveDistance)
	{
		TargetLocation = FVector::ZeroVector;
		return true;
	}
	else
	{
		// Check for other AI that might be on collision course
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		FHitResult hitResult;
		FCollisionShape shape;
		shape.SetSphere(SpheretraceRadius);
		if (GetWorld()->SweepSingleByChannel(hitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * LookForCollisionDistance, FQuat::Identity, ECollisionChannel::ECC_Visibility, shape, CollisionQueryParams))
		{
			GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
			TargetLocation = FVector::ZeroVector;
			return false;
		}

		GetCharacterMovement()->BrakingDecelerationFlying = 0.f;
		GetCharacterMovement()->Velocity = GetActorForwardVector() * FlySpeed;

		// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), TargetLocation, 25.f, FColor::Red, false, 1.f, 15.f);

		return false;
	}
}