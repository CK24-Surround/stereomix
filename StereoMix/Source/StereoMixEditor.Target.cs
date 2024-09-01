// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixEditorTarget : TargetRules
{
	public StereoMixEditorTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("StereoMix");

		StereoMixTarget.ApplySharedStereoMixTargetSettings(this);
	}
}
