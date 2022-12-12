// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIBaseController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAIBaseCharacter.h"
#include "DrawDebugHelpers.h"

AEnemyAIBaseController::AEnemyAIBaseController()
{
}

// Should this method misbehave anymore, just switch to using SetFocus and supply a sort of checkpoint-actor for the AI instead.....
void AEnemyAIBaseController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	APawn *const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();

		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}