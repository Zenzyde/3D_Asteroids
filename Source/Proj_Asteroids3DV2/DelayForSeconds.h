
#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"

class PROJ_ASTEROIDS3DV2_API FDelayForSeconds : public FPendingLatentAction
{
	FWeakObjectPtr Callback;

	FName ExecutionFunction;

	float ElapsedTime;
	float RemainingTime;

	float TotalTime;

	int ActionID;
	int PinNum;

public:
	FDelayForSeconds(float WaitTime, FLatentActionInfo &Info);

	virtual void UpdateOperation(FLatentResponse &Response) override;

	float GetElapsedTime() const;
	float GetRemainingTime() const;
	float GetElapsedTimePercent() const;
	float GetElapsedTimePercentInversed() const;
	int GetActionID() const;
};