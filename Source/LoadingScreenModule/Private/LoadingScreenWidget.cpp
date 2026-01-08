// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenWidget.h"
#include "LoadingScreenModule.h"

void ULoadingScreenWidget::EndLoadingScreen()
{
	// Close the Loading Screen
	if (LoadingScreenModule)
	{
		LoadingScreenModule->EndLoadingScreen();
	}
}

void ULoadingScreenWidget::SetLoadingScreenModule(class FLoadingScreenModule* NewModule)
{
	LoadingScreenModule = NewModule;
}

void ULoadingScreenWidget::ResetWidgetContent()
{
	ResetContent();
}
