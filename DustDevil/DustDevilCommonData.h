// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(DDWarning, Warning, All)
DECLARE_LOG_CATEGORY_EXTERN(DDLog, Log, All)

#include "CoreMinimal.h"
#include "DustDevilCommonData.generated.h"

UENUM()
enum POIType : uint8 
{
	COMMON						UMETA(DisplayName="COMMON"),
	UNCOMMON					UMETA(DisplayName="UNCOMMON"),
	RARE						UMETA(DisplayName="RARE"),
	EPIC						UMETA(DisplayName="EPIC"),
	LEGENDARY					UMETA(DisplayName="LEGENDARY")
};

/*
 * This enum represents the Tier of the Enemy, this should correlate generally with size health and reward.
 * This is also used to prevent Bosses and Special Enemy Types from being automatically added to attack groups.
 */
UENUM(BlueprintType)
enum EEnemyTier : uint8
{
	// Can Spawn Randomly
	ET_FODDER						UMETA(DisplayName="Fodder"),
	// Can Spawn Randomly
	ET_STANDARD						UMETA(DisplayName="Standard"),
	// Can Spawn Randomly
	ET_ELITE						UMETA(DisplayName="Elite"),
	// Can not Spawn Randomly
	ET_BOSS							UMETA(DisplayName="Boss"),
	// Can not Spawn Randomly
	ET_SPECIAL						UMETA(DisplayName="Special")
};

// A bitmasked enumerator for Different Types of movement available to the Enemy
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EEnemyTypes : uint8
{
	ETY_NONE = 0				UMETA(DisplayName = "None",			ToolTip = "Default value, Needs to be Overwritten"),
	ETY_GROUND = 1 << 0			UMETA(DisplayName = "Ground",		ToolTip = "Enemy can crawl on the ground"),
	ETY_AIR = 1 << 1			UMETA(DisplayName = "Air",			ToolTip = "Enemy can fly in the air"),
	ETY_BURROW = 1 << 2			UMETA(DisplayName = "Burrow",		ToolTip = "Enemy can burrow underground")
};
ENUM_CLASS_FLAGS(EEnemyTypes)

USTRUCT(BlueprintType)
struct FEnemyBaseStats
{
	GENERATED_BODY()

	// The Amount of Health the enemy has at base, before difficulty scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta = (ClampMin = 0.1f))
	float Health = 1.f;

	// The Amount of Armour the enemy has at base, before difficulty scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats")
	float Armour = 0.f;

	// The Amount of Damage the enemy deals at base, before difficulty scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta = (ClampMin = 0))
	float Damage = 1.f;

	// The Rate at which the enemy attacks its target, in attacks per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta = (ClampMin = 0.001f))
	float AttackSpeed = 1.f;
	
	// This one isn't in GAS, but is the base movement speed of the enemy. Is multiplied when the difficulty is raised
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta=(Units = "CentimetersPerSecond", ClampMin = 0.f))
	float BaseSpeed = 600.f;

	// The Amount of BioMatter the enemy drops on death by default (before modifications)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats")
	float BaseBioMatter = 1.f;

	// The Chance the Enemy has to Drop Nuclear material (0 to 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta = (ClampMin = 0, ClampMax = 1))
	float NuclearMaterialChance = 0.f;
	
	// The chance that the enemy drops Mutated Chitin (0 to 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyStats", meta = (ClampMin = 0, ClampMax = 1))
	float ChitinDropChance = 0.f;
};

USTRUCT(BlueprintType)
struct DUSTDEVIL_API FEnemyTemplateTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// The Sub Class of enemy, ie Enemy1 (Ground), Enemy2 (Flying) or Enemy3 (Burrow)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	TSubclassOf<class AEnemyBase> EnemySubClass;

	// The Base Stats of the enemy, Modifications on the base Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	FEnemyBaseStats BaseStats;

	// The Rate at which the Difficulty Scaling is applied	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData", meta = (ClampMin = 0))
	float DifficultyCoefficient = 1.f;
	
	// A Multiplier applied to the enemy on spawn, to make larger variants of enemies by default (This Value also Scales based on health).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	float Scale = 1.f;

	// How much this unit 'Costs' to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	float CostValue = 1.f;

	/*
	* This enum represents the Tier of the Enemy, this should correlate generally with size health and reward.
	* This is also used to prevent Bosses and Special Enemy Types from being automatically added to attack groups.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	TEnumAsByte<EEnemyTier> Tier = EEnemyTier::ET_STANDARD;

	/*
	* The "Type" Associated with the enemy for spawning, used as a filter for Attack Groups spawning types of enemies.
	* i.e. An Attack group may only be allowed to spawn Grounded or Burrowing enemies, which will blacklist Flying
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData", meta = (AllowPrivateAccess=true, Bitmask="EnemyTypes", BitmaskEnum = "/Script/DustDevil.EEnemyTypes"))
	int32 EnemyTypes = 0;
	
	// This can be left blank, but is here just in-case some enemies have abilities, and should use them (projectiles, novas, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	TArray<TSubclassOf<class UGameplayAbility>> StartingAbilities;

	// This can be left blank, but is here just in-case some enemies have effects (OnFire as an example)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyData")
	TArray<TSubclassOf<class UGameplayEffect>> StartingEffects;
};

/**
 * 
 */
class DUSTDEVIL_API DustDevilCommonData
{
public:
	DustDevilCommonData();
	~DustDevilCommonData();
};
