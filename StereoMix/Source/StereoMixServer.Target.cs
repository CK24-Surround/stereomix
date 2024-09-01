// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixServerTarget : TargetRules
{
	public StereoMixServerTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Server;
		ExtraModuleNames.Add("StereoMix");

		StereoMixTarget.ApplySharedStereoMixTargetSettings(this);
	}
}