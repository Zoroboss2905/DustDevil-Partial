#pragma once
#include "Modules/ModuleInterface.h"

// Loading Screen Module Implementation
// Handles initialisation and display of loading screens during gameplay
class FLoadingScreenModule : public IModuleInterface
{
public:
	// Called when the module is first loaded
	virtual void StartupModule() override;

	// Identifies this as a Game Module, not an Editor Module
	virtual bool IsGameModule() const override;

	// Shows the loading screen during level transition
	virtual void StartLoadingScreen(const FString& MapName, class ULoadingScreenWidget* InWidget = nullptr);

	void EndLoadingScreen();
	
private:
	// Store the background Texture to prevent garbage collection
	UTexture2D* BackgroundTexture;

	// Store the Widget to prevent Garbage Collection
	TSubclassOf<class UUserWidget> LoadingScreenWidget;	
};