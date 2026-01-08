// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreen.h"
#include "SlateOptMacros.h"
#include "SlateExtras.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLoadingScreen::Construct(const FArguments& InArgs)
{
	// Store texture from Arguments
	BackgroundTexture = InArgs._BackgroundTexture;

	// Create Brush for rendering background
	BackgroundBrush = MakeShareable(new FSlateBrush);
	BackgroundBrush->SetResourceObject(BackgroundTexture);

	// Widget Hierarchy
	ChildSlot
	[
		// Create an Overlay widget which allows stacking multiple elements
		SNew(SOverlay)
			// Add first slot to overlay for the background image
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)	// Stretch Horizontally to fill space
		.VAlign(VAlign_Fill)	// Stretch Vertically to fill space
		[
			// Create an Image widget using out background brush
			SNew(SImage)
			.Image(BackgroundTexture ? BackgroundBrush.Get() : nullptr)
			.ColorAndOpacity(FLinearColor::White)
		]

		// Add another Overlay for the Throbber
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)		// Align to bottom
		.HAlign(HAlign_Right)		// Align to right
		.Padding(10.f)				// Pad by 10 units, so it's off the corner
		[
			SNew(SThrobber)
			.Visibility(EVisibility::HitTestInvisible)		// Visible but doesnt block clicks
			.NumPieces(10)									// Number of spinning pieces in the throbber
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
