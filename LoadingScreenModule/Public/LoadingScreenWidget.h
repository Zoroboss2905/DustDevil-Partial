// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOADINGSCREENMODULE_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Callable Function to explicitly end the loading screen
	UFUNCTION(BlueprintCallable, Category = "LoadingScreen")
	void EndLoadingScreen();

	UFUNCTION(BlueprintImplementableEvent, Category = "LoadingScreen")
	void ResetContent();
	
public:
	void SetLoadingScreenModule(class FLoadingScreenModule* NewModule);

	void ResetWidgetContent();
	
private:
	// Reference to LoadingScreenModule
	class FLoadingScreenModule* LoadingScreenModule;
};
