// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixEditorTarget : TargetRules
{
	public StereoMixEditorTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange(new[] { "StereoMix", "StereoMixEditor" });

		if (!bBuildAllModules)
		{
			NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		}

		StereoMixTarget.ApplySharedStereoMixTargetSettings(this);
	}
}