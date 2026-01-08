// Fill out your copyright notice in the Description page of Project Settings.


#include "DustDevilGameInstance.h"
#include "LoadingScreenModule.h"
#include "LoadingScreenWidget.h"
#include "Blueprint/UserWidget.h"
#include "DustDevil/DustDevilCommonData.h"

void UDustDevilGameInstance::Init()
{
	Super::Init();

	// Bind pre-map and post-map loading Delegates
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UDustDevilGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UDustDevilGameInstance::EndLoadingScreen);

	if(FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule"))
	{
		// Spawn a LoadingScreenWidget instance
		// LoadingScreenWidgetInstance = MakeWidget(LoadingScreenModule);
	}
}

void UDustDevilGameInstance::BeginLoadingScreen(const FString& MapName)
{
	UE_LOG(DDLog, Warning, TEXT("[DustDevilGameInstance]: BeginLoadingScreen Started: %s"), *MapName);
	// If this is a Client (Or Server) that needs to bother with loading screens
	if (!IsRunningDedicatedServer())
	{
		// Try to get the LoadingScreenModule
		if(FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule"))
		{
			if (!LoadingScreenWidgetInstance)
			{
				LoadingScreenWidgetInstance = MakeWidget(LoadingScreenModule);
			}
			LoadingScreenModule->StartLoadingScreen(MapName, LoadingScreenWidgetInstance);
		}
		else
		{
			// If Failure, Output and error and move on
			UE_LOG(DDLog, Error, TEXT("[DustDevilGameInstance]: LoadingScreenModule Not Found! %s"), *MapName);
		}
	}
}

void UDustDevilGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	// TODO: Add extra functionality here for what to do with the Loaded World
	UE_LOG(DDLog, Warning, TEXT("[DustDevilGameInstance]: EndLoadingScreen: %s"), *InLoadedWorld->GetName());
}

ULoadingScreenWidget* UDustDevilGameInstance::MakeWidget(FLoadingScreenModule* LoadingScreenModule)
{
	ULoadingScreenWidget* NewWidget = nullptr;
	if (IsValid(LoadingScreenWidget))
	{
		// Get the default object of this loading screen, create and instance of it and supply that to the LoadingScreenModule
		// TimerManager->SetTimerForNextTick()
		NewWidget = CreateWidget<ULoadingScreenWidget>(this, LoadingScreenWidget);
		// Setup LoadingScreenWidget reference
		NewWidget->SetLoadingScreenModule(LoadingScreenModule);
	}
	else
	{
		UE_LOG(DDLog, Error, TEXT("Loading Screen Widget is *somehow* invalid"))
	}
	return NewWidget;
}
