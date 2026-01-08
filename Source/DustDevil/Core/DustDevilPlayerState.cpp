// Fill out your copyright notice in the Description page of Project Settings.


#include "DustDevilPlayerState.h"
#include "DustDevil/GAS/VehicleAttributeSet.h"
#include "DustDevil/GAS/DD_AbilitySystemComponent.h"

ADustDevilPlayerState::ADustDevilPlayerState()
{
	// Create Ability System Component and Set Replication
	AbilitySystemComponent = CreateDefaultSubobject<UDD_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// Replication for multiplayer (if we use it later)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	NetUpdateFrequency = 100.f;

	// Initialise the Attribute set
	AttributeSet = CreateDefaultSubobject<UVehicleAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ADustDevilPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UVehicleAttributeSet* ADustDevilPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
