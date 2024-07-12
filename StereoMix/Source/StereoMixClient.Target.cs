// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixClientTarget : TargetRules
{
	public StereoMixClientTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("StereoMix");

		ProjectDefinitions.Add("UE_ALLOW_MAP_OVERRIDE_IN_SHIPPING");
	}
}