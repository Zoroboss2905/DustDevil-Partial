// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Vehicle.h"

void AVehicleController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	// Replication
	
	// Cast character from pawn
	AVehicle* Vehicle = Cast<AVehicle>(P);
	if (Vehicle)
	{
		Vehicle->GetAbilitySystemComponent()->InitAbilityActorInfo(Vehicle, Vehicle);
	}
}

void AVehicleController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Get Enhanced Player input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Add Mapping Context at top priority (Notify User Settings to allow for custom remapping)
		FModifyContextOptions InputContextOptions;
		InputContextOptions.bNotifyUserSettings = true;

		Subsystem->AddMappingContext(InputMappingContext, 0, InputContextOptions);
	}
}

void AVehicleController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Get a pointer to the possessed pawn
	VehiclePawn = Cast<AVehicle>(InPawn);
}

void AVehicleController::OnUnPossess()
{
	Super::OnUnPossess();

	// Set Vehicle Pawn to nullptr
	VehiclePawn = nullptr;
}
