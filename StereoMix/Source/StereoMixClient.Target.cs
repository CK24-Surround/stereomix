// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMixClientTarget : TargetRules
{
	public StereoMixClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("StereoMix");
		
		// 커스텀 서버 접속을 위해 Shipping 빌드에서 맵 오버라이드 허용
		ProjectDefinitions.Add("UE_ALLOW_MAP_OVERRIDE_IN_SHIPPING");
	}
}
