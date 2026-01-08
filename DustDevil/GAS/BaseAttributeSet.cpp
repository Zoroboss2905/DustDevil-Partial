// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UBaseAttributeSet::UBaseAttributeSet()
{
	InitDefaultValues();
}

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Armour, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, BioMatter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, NuclearMatter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, ChitinChance, COND_None, REPNOTIFY_Always);
}

void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldValue);
}

void UBaseAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, CurrentHealth, OldValue);
}

void UBaseAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, HealthRegen, OldValue);
}

void UBaseAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Damage, OldValue);
}

void UBaseAttributeSet::OnRep_Armour(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Armour, OldValue);
}

void UBaseAttributeSet::OnRep_BioMatter(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, BioMatter, OldValue);
}

void UBaseAttributeSet::OnRep_NuclearMatter(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, NuclearMatter, OldValue);
}

void UBaseAttributeSet::OnRep_ChitinChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, ChitinChance, OldValue);
}

void UBaseAttributeSet::InitDefaultValues(float NewMaxHealth, float NewCurrentHealth, float NewHealthRegen,
                                          float NewDamage, float NewArmour, float NewBioMatter, float NewNuclearMaterial)
{
	// Set Default Values (Automatically set to 1 for health and damage, but 0 for everything else)
	InitMaxHealth(NewMaxHealth);
	InitCurrentHealth(NewCurrentHealth);
	InitHealthRegen(NewHealthRegen);
	InitDamage(NewDamage);
	InitArmour(NewArmour);
	InitBioMatter(NewBioMatter);
	InitNuclearMatter(NewNuclearMaterial);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// Which values should be clamped?
	/*
	 *	Current Health should not drop below 0 or exceed MaxHealth
	 *
	 *	Both Currency Values should not drop below 0
	 *	 
	 */

	// Clamp Health
	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}

	// Minimum BioMatter is 0
	if (Attribute == GetBioMatterAttribute())
	{
		NewValue = NewValue < 0.f ? 0.f : NewValue;
	}

	// Minimum NuclearMatter is 0
	if (Attribute == GetNuclearMatterAttribute())
	{
		NewValue = NewValue < 0.f ? 0.f : NewValue;
		
	}
}

void UBaseAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp Health
	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}
	
	// Minimum BioMatter is 0
	if (Attribute == GetBioMatterAttribute())
	{
		NewValue = NewValue < 0.f ? 0.f : NewValue;
	}

	// Minimum NuclearMatter is 0
	if (Attribute == GetNuclearMatterAttribute())
	{
		NewValue = NewValue < 0.f ? 0.f : NewValue;
		
	}
}

void UBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	// Clamp Health when Max Health Changes
	if (Attribute  == GetMaxHealthAttribute())
	{
		// If current Health is above the MaxHealth value
		if (GetCurrentHealth() > NewValue)
		{
			// Set current health to Max Health (clamp health from radiation damage)
			SetCurrentHealth(NewValue);
		}
	}
}

void UBaseAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
	// Clamp Health if Needed?
}
