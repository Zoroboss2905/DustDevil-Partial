// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DustDevilTarget : TargetRules
{
	public DustDevilTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("DustDevil");
	
		// Enable Console and Cheat Manager in Shipping Builds
		// GlobalDefinitions.Add("ALLOW_CONSOLE_IN_SHIPPING=1");
		// GlobalDefinitions.Add("UE_WITH_CHEAT_MANAGER=1");
	}
}
