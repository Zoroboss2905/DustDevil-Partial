// Fill out your copyright notice in the Description page of Project Settings.


#include "DD_AbilitySystemComponent.h"

UDD_AbilitySystemComponent::UDD_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// For Multiplayer replication, if needed later
	SetIsReplicated(true);
}