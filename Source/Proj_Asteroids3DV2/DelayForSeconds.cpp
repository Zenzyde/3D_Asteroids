
#include "DelayForSeconds.h"
#include "LatentActions.h"
#include "Engine/Engine.h"

FDelayForSeconds::FDelayForSeconds(float WaitTime, FLatentActionInfo &Info) : RemainingTime(WaitTime), TotalTime(WaitTime), ActionID(Info.UUID), PinNum(Info.Linkage), ExecutionFunction(Info.ExecutionFunction), Callback(Info.CallbackTarget)
{
}

void FDelayForSeconds::UpdateOperation(FLatentResponse &Response)
{
	RemainingTime -= Response.ElapsedTime();
	ElapsedTime += Response.ElapsedTime();

	Response.FinishAndTriggerIf(RemainingTime <= 0.f, ExecutionFunction, PinNum, Callback);
}

float FDelayForSeconds::GetElapsedTime() const
{
	return ElapsedTime;
}

float FDelayForSeconds::GetRemainingTime() const
{
	return RemainingTime;
}

float FDelayForSeconds::GetElapsedTimePercent() const
{
	return (ElapsedTime / TotalTime);
}

float FDelayForSeconds::GetElapsedTimePercentInversed() const
{
	return (RemainingTime / TotalTime);
}

int FDelayForSeconds::GetActionID() const
{
	return ActionID;
}