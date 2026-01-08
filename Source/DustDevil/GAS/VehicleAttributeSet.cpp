// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleAttributeSet.h"

UVehicleAttributeSet::UVehicleAttributeSet()
{
	InitDefaultValues();
}

void UVehicleAttributeSet::InitDefaultVehicleValues(float NewMaxHealth, float NewCurrentHealth, float NewDamage,
	float NewArmour, float NewBioMatter, float NewNuclearMaterial, float NewHandling, float NewTopSpeed,
	float NewAccelerationModifier, float NewRadShield, float NewMaxFuelCapacity)
{
	// Initialise base values as parent
	InitDefaultValues(NewMaxHealth, NewCurrentHealth, NewDamage, NewArmour, NewBioMatter, NewNuclearMaterial);

	// Initialise Vehicle Base values
	InitHandling(NewHandling);
	InitTopSpeed(NewTopSpeed);
	InitAccelerationModifier(NewAccelerationModifier);
	InitRadShield(NewRadShield);
	InitMaxFuelCapacity(NewMaxFuelCapacity);
	InitBioFuel(0.f);
	InitMutatedChitin(0.f);
	InitRammingDamage(1.f);
	InitMaxBoost(0.f);
	InitCurrentBoost(0.f);
	InitMaxBulletTime(0.f);
	InitCurrentBulletTime(0.f);
	InitTankBioBonus(1.f);
	InitTankPermaBonus(1.f);
	InitTankConvertBonus(1.f);

	// All Temporary Stats init to 0
	// Offence
	InitFuryDamage(0.f);
	InitCriticalChance(0.f);
	InitCriticalDamage(0.f);
	InitReloadSpeedBonus(0.f);
	InitRageDamage(0.f);
	InitGlassCannonDamage(0.f);
	InitGlassCannonHealth(0.f);
	// Defence
	InitGritDefence(0.f);
	InitLifeSteal(0.f);
	InitBioHealth(0.f);
	InitSpeedDefence(0.f);
	InitImmunityTime(0.f);
	// Support
	InitTrickChance(0.f);
	InitThornsPercent(0.f);
	InitBoostHealing(0.f);
	InitSuctionChance(0.f);
	InitKeepsakePercent(0.f);
}

void UVehicleAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Which values should be clamped?
	/*
	 *	Current values (Boost and Bullet Time) should clamp 0 - max
	 *
	 */

	// Clamp Boost
	if (Attribute == GetCurrentBoostAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxBoost());
	}

	// Clamp Bullet Time
	if (Attribute == GetCurrentBulletTimeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxBulletTime());
	}
}

void UVehicleAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Also Clamp Base values (Specifically for Regenerating cooldowns on Boost and Bullet Time)

	// Clamp Boost
	if (Attribute == GetCurrentBoostAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxBoost());
	}

	// Clamp Bullet Time
	if (Attribute == GetCurrentBulletTimeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxBulletTime());
	}
}
