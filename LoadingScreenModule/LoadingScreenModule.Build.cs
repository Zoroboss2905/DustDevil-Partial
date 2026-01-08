using UnrealBuildTool;

// Critical: Module must have "PreLoadingScreen" load timing in .uproject
// Otherwise: Module will ont initialise early enough to handle loading screens

public class LoadingScreenModule : ModuleRules
{
	public LoadingScreenModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "MoviePlayer" });
		// Specify Primary header file for precompiled headers
		PrivatePCHHeaderFile = "Public/LoadingScreenModule.h";
		
		// Use shared precompiled headers for better compile times
		PCHUsage = PCHUsageMode.UseSharedPCHs;
		
		// Add core Unreal Engine modules as Private Dependencies
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",				// Core UE Functionality
				"CoreUObject",		// Base UObject System
				"MoviePlayer",		// Movie player for Loading Screens
				"Engine",			// Main UE Module for UTexture2D
				"Slate",			// UI Framework
				"SlateCore",		// Core Slate Functionality
				"UMG"				// User Widget Functionality?
			}
		);
	}
}