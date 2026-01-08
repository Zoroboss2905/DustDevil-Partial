// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// Damage Reduction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Armour, Category = "Damage")
	FGameplayAttributeData Armour;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Armour)

	// Biomatter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BioMatter, Category = "Currency")
	FGameplayAttributeData BioMatter;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, BioMatter)

	// Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHealth, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CurrentHealth)

	// Basic Damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

	// Max Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

	// Health Regeneration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegen, Category = "Health")
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, HealthRegen)

	// Nuclear Matter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_NuclearMatter, Category = "Currency")
	FGameplayAttributeData NuclearMatter;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, NuclearMatter)
	
	// Chitin Drop Chance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ChitinChance, Category = "Currency")
	FGameplayAttributeData ChitinChance;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, ChitinChance)

public:
	UBaseAttributeSet();

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Replication Variables
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_HealthRegen(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_Armour(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_BioMatter(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_NuclearMatter(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_ChitinChance(const FGameplayAttributeData& OldValue) const;

public:
	// Initialise Default Values for Base Class
	UFUNCTION(BlueprintCallable, Category = "BaseStat")
	void InitDefaultValues(float NewMaxHealth = 1.f, float NewCurrentHealth = 1.f, float NewHealthRegen = 0.f, float NewDamage = 1.f, float NewArmour = 0.f, float NewBioMatter = 0.f, float NewNuclearMaterial = 0.f);

	// Clamp Values BEFORE adjusting them
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Clamp Values AFTER adjusting them
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
};
