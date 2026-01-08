// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DustDevilPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API ADustDevilPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADustDevilPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual class UVehicleAttributeSet* GetAttributeSet() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerData|Abilities")
	class UDD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerData|Abilities")
	UVehicleAttributeSet* AttributeSet;

private:
	// Refined Biomass, used as Experience for granting temporary upgrades to the Player. (Removed on Level Switch)
	UPROPERTY(BlueprintReadWrite, Category = "Currency", meta = (AllowPrivateAccess = true))
	float BioFuel;
};
