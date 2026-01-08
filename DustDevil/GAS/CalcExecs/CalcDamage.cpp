// Fill out your copyright notice in the Description page of Project Settings.


#include "CalcDamage.h"

#include "GameplayTagsManager.h"
#include "DustDevil/DustDevilCommonData.h"
#include "../VehicleAttributeSet.h"


// Captured Attributed for Damage Calculations
struct DamageCapture
{
	// TODO: Re-Work damage calculations as more attributes need to be handled
	// Source Data
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	
	// Target Data
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armour);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ThornsPercent);
	DECLARE_ATTRIBUTE_CAPTUREDEF(GritDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SpeedDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ImmunityTime);

	DamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Damage, Source, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, CriticalChance, Source, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, CriticalDamage, Source, true)
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, CurrentHealth, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, MaxHealth, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Armour, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, ThornsPercent, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, GritDefence, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, SpeedDefence, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVehicleAttributeSet, ImmunityTime, Target, true)
	}
};

// Static Function for DamageCapture Struct
static DamageCapture& GetDamageCapture()
{
	static DamageCapture DamageCapture;
	return DamageCapture;
}

UCalcDamage::UCalcDamage()
{
	// Capture attributes
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().CriticalChanceDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().CriticalDamageDef);
	
	RelevantAttributesToCapture.Add(GetDamageCapture().CurrentHealthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().MaxHealthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().ArmourDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().ThornsPercentDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().GritDefenceDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().ImmunityTimeDef);
}
	
void UCalcDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Damage Calculation Prep
#pragma region Function Setup
	// Get the Target Ability system component and their respective actor
	UAbilitySystemComponent* TargetABSC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetABSC ? TargetABSC->GetAvatarActor() : nullptr;

	// Get the Source Ability system component and their respective actor

	UAbilitySystemComponent* SourceABSC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceABSC ? SourceABSC->GetAvatarActor() : nullptr;

	// Get Specifications and Tags
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get all Tags
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Instance each captured variable as a localised variable, then fetch them from the captured attributes
	float TargetCurrentHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().CurrentHealthDef, EvaluationParameters, TargetCurrentHealth);

	float TargetMaxHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().MaxHealthDef, EvaluationParameters, TargetMaxHealth);
	
	float TargetArmour = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().ArmourDef, EvaluationParameters, TargetArmour);

	float TargetThornsPercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().ThornsPercentDef, EvaluationParameters, TargetThornsPercent);
	
	float SourceDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageDef, EvaluationParameters, SourceDamage);
	
	float SourceCritChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().CriticalChanceDef, EvaluationParameters, SourceCritChance);

	float SourceCritDamage = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().CriticalDamageDef, EvaluationParameters, SourceCritDamage);

	float TargetGritDef = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().GritDefenceDef, EvaluationParameters, TargetGritDef);
	
	float TargetSpeedDef = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().SpeedDefenceDef, EvaluationParameters, TargetSpeedDef);
	
	float TargetImmunityTime = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().ImmunityTimeDef, EvaluationParameters, TargetImmunityTime);
	// Print an Error and do not perform calculations
	if (!IsValid(TargetActor))
	{
		UE_LOG(DDLog, Error, TEXT("Target Actor not valid for Damage Calculation"));
		return;
	}
	if (!IsValid(SourceActor))
	{
		UE_LOG(DDLog, Error, TEXT("Source Actor not valid for Damage Calculation"));
		return;
	}

#pragma endregion 

#pragma region Damage Calculation
	// Used during calculation, the amount of health removed from the target

	bool bCritSuccess = FMath::RandRange(0.f, 100.f) < SourceCritChance;
	
	// TODO: Integrate all relevant temporary upgrades
	// Add all Damage Bonuses first
	/*
	 * Damage Incoming is already modified by Fury
	 * Multiply Damage by CritDamage if CritChance is a success
	 */
	float FinalDamage = SourceDamage; 
	FinalDamage = FinalDamage * (bCritSuccess ? SourceCritDamage : 1.f);

	// Apply all damage reductions
	/*
	 *	Reduce Damage by flat amount (Armour)
	 *	Reduce Damage by flat amount (SpeedDefence)
	 *	Then reduce Damage by a percentage (GritDefence)
	 */
	// Armour reduces damage by a flat value
	FinalDamage -= TargetArmour;
	// SpeedDefence reduces damage by a flat value
	FinalDamage -= TargetSpeedDef;
	// Then GritDefence reduces Damage by a percentage
	FinalDamage = FinalDamage * (1 - (TargetGritDef / 100.f));
	// Ensure Damage Value is not negative (Healing)
	FinalDamage = FMath::Max(FinalDamage, 0.f);
	
	// Invert Final Damage number before applying damage (Damage is negative)
	FinalDamage = -FinalDamage;
	
	// Calculate Thorns damage
	float ThornsDamage = FinalDamage * (TargetThornsPercent / 100.f);
	
	// TODO: Determine whether or not the Pre and post Gameplay effects should be triggered here
	// Modify Attribute through Ability System Component (This Triggers Pre and Post Gameplay Effects)
	TargetABSC->ApplyModToAttribute(UBaseAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Additive, FinalDamage);

	// If Thorns damage active apply ThornsDamage to the Source Actor
	if (ThornsDamage < 0.f)
	{
		SourceABSC->ApplyModToAttribute(UBaseAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Additive, ThornsDamage);
	}
	
	// If a Critical Hit was successfully triggered.
	if (bCritSuccess)
	{
		// Apply GameplayCue for taking crit damage
		FGameplayTag CritCueTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("GameplayCue.Damage.Crit"));
		// Remove Existing Gameplay Cue, replace with a new one.
		TargetABSC->RemoveGameplayCue(CritCueTag);
		TargetABSC->AddGameplayCue(CritCueTag);
	}

	// Handle the application of Immunity
	if (TargetImmunityTime > 0.f)
	{
		// TODO: Handle Immunity
		// Add loose 'Identifier' tag to the player, so that it may consume it and activate an effect for damage immunity
	}
	
#pragma endregion 

}
