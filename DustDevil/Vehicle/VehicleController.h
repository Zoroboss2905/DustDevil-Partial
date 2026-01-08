// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VehicleController.generated.h"

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API AVehicleController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void AcknowledgePossession(class APawn* P) override;

protected:

	// Input Mapping Context for player input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputMappingContext* InputMappingContext;

	// Controlled Vehicle pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle")
	class AVehicle* VehiclePawn;

protected:
	
	// Runs when Vehicle Spawned, sets up input scheme.
	virtual void SetupInputComponent() override;

	// Runs when player possesses a Pawn, allocates pawn
	virtual void OnPossess(APawn* InPawn) override;
	// Runs when player un-possesses a Pawn, de-allocates pawn
	virtual void OnUnPossess() override;

};
