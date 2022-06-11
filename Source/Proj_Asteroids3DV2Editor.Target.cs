// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Proj_Asteroids3DV2EditorTarget : TargetRules
{
	public Proj_Asteroids3DV2EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("Proj_Asteroids3DV2");
	}
}
