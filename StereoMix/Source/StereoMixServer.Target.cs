// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixServerTarget : TargetRules
{
	public StereoMixServerTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("StereoMix");
	}
}
