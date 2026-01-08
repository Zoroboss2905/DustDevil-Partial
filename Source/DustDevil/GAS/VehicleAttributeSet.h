// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VehicleAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API UVehicleAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	// Acceleration Modifier (The Rate at which the car reaches top speed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VehicleMovement")
	FGameplayAttributeData AccelerationModifier;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, AccelerationModifier);

	// Handling (The Size of the Turning Circle and the Turning Speed of the vehicle)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VehicleMovement")
	FGameplayAttributeData Handling;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, Handling);

	// Maximum amount of BioMass the Vehicle can store at a time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData MaxFuelCapacity;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, MaxFuelCapacity);

	// The Amount of BioFuel stored in the Deposit Tanks (Player Instanced, so it is here.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData BioFuel;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, BioFuel);

	// A multiplier to the amount of biofuel gained when picking up biomatter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData TankBioBonus;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, TankBioBonus);

	// A Multiplicative bonus to the nuclear matter drop chance 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData TankPermaBonus;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, TankPermaBonus);

	// A Multiplier to the conversion rate of biomatter to biofuel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData TankConvertBonus;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, TankConvertBonus);

	// Amount of MutatedChitin Gathered this Run
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	FGameplayAttributeData MutatedChitin;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, MutatedChitin);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData RadShield;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, RadShield);

	// Top Speed (The maximum speed of the vehicle)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VehicleMovement")
	FGameplayAttributeData TopSpeed;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, TopSpeed);

	// Damage Value Separated from weapon damage, for the explicit purpose of ramming.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData RammingDamage;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, RammingDamage);

	// The Maximum Value for the players boost ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActiveAbility")
	FGameplayAttributeData MaxBoost;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, MaxBoost);

	// The Current Value for the players boost ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActiveAbility")
	FGameplayAttributeData CurrentBoost;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, CurrentBoost);

	// The Maximum Value for the players bullet time ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActiveAbility")
	FGameplayAttributeData MaxBulletTime;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, MaxBulletTime);

	// The Current Value for the players bullet time ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActiveAbility")
	FGameplayAttributeData CurrentBulletTime;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, CurrentBulletTime);

	// Temporary Upgrade Stats
	// Offense

	// Stacking Damage Increase, this amount is the multiplier per stack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData FuryDamage;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, FuryDamage);

	// Chance to Land a Critical Hit when Damaging an Enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, CriticalChance);

	// Multiplier to Damage dealt when landing a Critical Hit
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, CriticalDamage);

	// Multiplier to the tick-rate of weapon reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData ReloadSpeedBonus;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, ReloadSpeedBonus);

	// Additive Multiplier per stack of Road Rage (More stacks based on Health Value)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData RageDamage;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, RageDamage);

	// Percentage Multiplier to Damage output
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData GlassCannonDamage;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, GlassCannonDamage);

	// Percentage reduction to Max Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData GlassCannonHealth;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, GlassCannonHealth);

	// Defence

	// Damage Reduction Percentage (More stacks based on Health Value)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defence")
	FGameplayAttributeData GritDefence;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, GritDefence);

	// Flat Health Reduction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defence")
	FGameplayAttributeData LifeSteal;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, LifeSteal);

	// Heal flat amounts of health based on incoming biomass
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defence")
	FGameplayAttributeData BioHealth;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, BioHealth);

	// Flat Damage reduction, based on the speed of the vehicle (0% - 100% for amount of speed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defence")
	FGameplayAttributeData SpeedDefence;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, SpeedDefence);
		
	// Duration of Immunity Effect applied when taking damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defence")
	FGameplayAttributeData ImmunityTime;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, ImmunityTime);

	// Support

	// Chance to Not Consume Ammunition when shooting.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Support")
	FGameplayAttributeData TrickChance;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, TrickChance);

	// Reflect a percentage of incoming damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Support")
	FGameplayAttributeData ThornsPercent;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, ThornsPercent);

	// Restore a portion of health when boosting shortly after damage (Maximum percent of health gained in this way)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Support")
	FGameplayAttributeData BoostHealing;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, BoostHealing);

	// Chance to Spawn an extra unit of BioMatter when landing shots on an enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Support")
	FGameplayAttributeData SuctionChance;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, SuctionChance);

	// Save a percentage of Pitchblende and Warped Chitin on death
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Support")
	FGameplayAttributeData KeepsakePercent;
	ATTRIBUTE_ACCESSORS(UVehicleAttributeSet, KeepsakePercent);
		
public:
	UVehicleAttributeSet();

public:
	UFUNCTION(BlueprintCallable, Category = "VehicleStats")
	void InitDefaultVehicleValues(float NewMaxHealth = 100.f, float NewCurrentHealth = 100.f, float NewDamage = 5.f, float NewArmour = 0.f, float NewBioMatter = 0.f, float NewNuclearMaterial = 0.f, 
		float NewHandling = 10.f, float NewTopSpeed = 10.f, float NewAccelerationModifier = 1.f, float NewRadShield = 0.f, float NewMaxFuelCapacity = 100.f);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
};
