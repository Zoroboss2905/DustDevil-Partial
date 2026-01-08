#include "LoadingScreenModule.h"
#include "SLoadingScreen.h"
#include "MoviePlayer.h"
#include "CoreGlobals.h"
#include "LevelLoadingSettings.h"
#include "LoadingScreenWidget.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SObjectWidget.h"

void FLoadingScreenModule::StartupModule()
{
	// Log when Module Initialises
	UE_LOG(LogTemp, Warning, TEXT("[FLoadingScreenModule]: StartupModule"));

	// Try to get loading screen settings
	if (ULevelLoadingSettings* Settings = GetMutableDefault<ULevelLoadingSettings>())
	{
		// Get Background Image path from settings
		const FSoftObjectPath& BGPath = Settings->BackgroundImage;
		// If a Background image is specified
		if (!BGPath.IsNull())
		{
			// Load the Texture
			BackgroundTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *BGPath.ToString()));
		}
	}
}

bool FLoadingScreenModule::IsGameModule() const
{
	// Identifies this as a runtime module, not an editor module
	return true;
}

void FLoadingScreenModule::StartLoadingScreen(const FString& MapName, ULoadingScreenWidget* InWidget)
{
	// Log when loading screen is requested
	UE_LOG(LogTemp, Warning, TEXT("[FLoadingScreenModule]: StartLoadingScreen"))

	// Get loading Screen Settings
	ULevelLoadingSettings* Settings = GetMutableDefault<ULevelLoadingSettings>();

	// Check that LoadingScreens are Enabled
	if (!Settings->bLoadingScreensEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FLoadingScreenModule]: Loading Screens are Disabled!"));
		return;
	}
	
	// Check if this map should have a loading screen
	bool bShouldShowLoadingScreen = false;
	for (const FSoftObjectPath& MapPath : Settings->MapsWithLoadingScreens)
	{
		// Check if the Current Map name is in the valid list
		if (MapPath.GetAssetPathString().Contains(MapName))
		{
			// Flag that the loading screen should be displayed and stop early
			bShouldShowLoadingScreen = true;
			break;
		}
	}

	// If the Loading Screen was never flagged as true
	if (!bShouldShowLoadingScreen)
	{
		// Send Log Message and exit function early
		UE_LOG(LogTemp, Warning, TEXT("[LoadingScreenModule]: Level: %s is not flagged to have a loading screen on entry."), *MapName)
		return;
	}
	
	// Create a Loading Screen object.
	FLoadingScreenAttributes LoadingScreen = FLoadingScreenAttributes();

	// The Loading Screen should display for at least 1 second
	LoadingScreen.MinimumLoadingScreenDisplayTime = Settings->MinimumLoadingScreenDisplayTime;
	// Do not auto-complete the loading screen, have a prompt to continue
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = Settings->bAutoClearOnLoadingComplete;
	// Loop Movies, to allow player to close the loading screen when it's done
	LoadingScreen.PlaybackType = Settings->PlaybackType;	
	// Add in other Conditions here (By default, movies are skippable, don't wait for manual stop, don't allow in early startup, don't allow engine tick and playback video as MT_Normal)
	LoadingScreen.bWaitForManualStop = Settings->bMovieWaitsForManualStop;
	
	// Add blank LoadingScreenMovie, to fix timings
	LoadingScreen.MoviePaths = Settings->MoviePaths;
		
	// Allocate Widget for the Loading Screen
	if (IsValid(InWidget))
	{
		InWidget->ResetContent();
		LoadingScreen.WidgetLoadingScreen = InWidget->TakeWidget();
		UE_LOG(LogTemp, Warning, TEXT("[LoadingScreenModule]: Attempted to Load %s and add to viewport."), *InWidget->GetName())
	}
	else
	{
		LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen).BackgroundTexture(BackgroundTexture);
	}
	
	// Setup Loading screen with above parameters
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

	// Play the movie list
	GetMoviePlayer()->PlayMovie();
}

void FLoadingScreenModule::EndLoadingScreen()
{
	GetMoviePlayer()->StopMovie();
	GetMoviePlayer()->ForceCompletion();
}

// Registers this Module with Unreal Engine's Module System
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);
