// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DustDevilGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API UDustDevilGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

private:
	UPROPERTY(BlueprintReadWrite, Category = "Loading Screen", meta = (AllowPrivateAccess = true))
	TSubclassOf<class ULoadingScreenWidget> LoadingScreenWidget;

	// LoadingScreenWidgetInstance
	UPROPERTY()
	class ULoadingScreenWidget* LoadingScreenWidgetInstance;
	
private:
	class ULoadingScreenWidget* MakeWidget(class FLoadingScreenModule* LoadingScreenModule);
	
};
