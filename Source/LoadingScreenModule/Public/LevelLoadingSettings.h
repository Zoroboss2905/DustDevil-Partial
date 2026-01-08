// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoviePlayer.h"
#include "Engine/DeveloperSettings.h"
#include "LevelLoadingSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Game", DefaultConfig, meta = (DisplayName = "Loading Screen"))
class LOADINGSCREENMODULE_API ULevelLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Are LoadingScreens Enabled? Disable this if loading screens are causing issues in Shipping Builds
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen")
	bool bLoadingScreensEnabled = true;
	// An Array of Soft references to map assets that should display loading screens
	// More specifically, when loading INTO these levels, a loading screen will be displayed
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "/Script/Engine.World", EditCondition = "bLoadingScreensEnabled"))
	TArray<FSoftObjectPath> MapsWithLoadingScreens;

	// The Path to the Background movie (should be the blank one)
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (EditCondition = "bLoadingScreensEnabled"))
	TArray<FString> MoviePaths;

	// Soft Reference to the Background Image asset for the loading screen
	// CAN BE REPLACED WITH A WIDGET, IF ALLOCATED IN THE GAME INSTANCE
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "/Script/Engine.Texture", EditCondition = "bLoadingScreensEnabled"))
	FSoftObjectPath BackgroundImage;
	
	// Minimum duration (in seconds) that the loading screen will be displayed
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (EditCondition = "bLoadingScreensEnabled"))
	float MinimumLoadingScreenDisplayTime = -1.f;

	// Automatically clear the Loading Screen when the game finishes loading
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (EditCondition = "bLoadingScreensEnabled"))
	bool bAutoClearOnLoadingComplete = true;

	// Movie Playback will continue playing until Stop() is called
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (EditCondition = "bLoadingScreensEnabled"))
	bool bMovieWaitsForManualStop = false;

	/* Automatic Playback type:
	 LoadingLoop plays all movies in order, then loops the last one
	 Looped Plays all Movies in order and loops back to the start of the list until stop is manually called
	 Normal plays all movies once
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (EditCondition = "bLoadingScreensEnabled"))
	TEnumAsByte<EMoviePlaybackType> PlaybackType = EMoviePlaybackType::MT_LoadingLoop;
};
