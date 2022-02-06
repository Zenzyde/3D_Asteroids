// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MiniProj_Asteroids : ModuleRules
{
	public MiniProj_Asteroids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
