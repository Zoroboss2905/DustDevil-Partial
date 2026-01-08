// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class DUSTDEVIL_API AEnemyManager : public AActor
{
	GENERATED_BODY()
public:
	// The Global Difficulty Value, 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Spawning")
	float GlobalDifficulty = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Debug")
	bool bDebugSymbols = false;	
public:	
	// Setup default values for enemy spawning (Data & Curve Tables)
	AEnemyManager();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/*
	This function Creates a New Attack Group object.
	It takes in:
		A Whitelist
		A SpawnLocation
		An Optional Array for a Preset Group
		An Optional AttackGroup Subclass
	and Outputs:
		A Complete AttackGroup
	The Whitelist limits the Type of enemies that can be spawned by this function. This is useless if the PresetGroup is used in any way.
	The SpawnLocation is a centerpoint of the spawnable area, the range increases with difficulty.
	The Optional PresetGroup fundamentally alters the functionality of the function.
		If ANY Enemy is in the PresetGroup, The Whitelist is completely ignored, as is the Difficulty Scaling specifically for the GroupSize.
			ALL enemies listed in the Preset group will be spawned, with their stats modified by the Difficulty Scaling
		If NO Enemies are supplied by the Preset Group, this function will randomly grab a row from the EnemyPresets Table,
			Determine if it meets the Cost and Whitelist categories. (Also is not a Special, or Boss Enemy)
			Then if true, spawns the enemy.
			Get another random row and try again until the group is full.
	The Optional Attack Group subclass allows for extra BP Functionality on AttackGroups, such as Objectives.
		Without a Subclass Listed, it will default to the c++ one, which has no bonus functionality
	The Complete AttackGroup is output AFTER being added to this EnemyManager's "AttackGroups" list.
	*/
	UFUNCTION(BlueprintCallable, Category = "Enemy|Management", meta = (AdvancedDisplay = 2, AutoCreateRefTerm = "PresetGroup"))
	void CreateAttackGroup(
		UPARAM(meta = (Bitmask = "EnemyTypes", BitmaskEnum = "/Script/DustDevil.EEnemyTypes")) int32 Whitelist,
		FVector SpawnLocation,
		TArray<struct FEnemyTemplateTableRow> PresetGroup,
		TSubclassOf<class AAttackGroup> CustomAttackGroupClass,
		class AAttackGroup*& NewAttackGroup);

	// Get a Row on the EnemyPresetDataTable by name (No Filtering)
	UFUNCTION(BlueprintPure, Category = "Enemy|Management")
	struct FEnemyTemplateTableRow GetEnemyPresetByName(FName TableRowName) const;

	// Called when an AttackGroup is Dissolved. AttackGroup is automatically Destroyed after this function completes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Management")
	void OnAttackGroupDissolved(class AAttackGroup* AttackGroup);
	
	// Called when an AttackGroup loses all of its Members
	UFUNCTION()
	void DissolveAttackGroup(class AAttackGroup* AttackGroup);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	// A List of all spawned Attack Groups
	UPROPERTY(BlueprintReadWrite, Category = "Enemy|Management", meta = (AllowPrivateAccess = true))
	TArray<class AAttackGroup*> AttackGroups;

	// The Data table associated with Enemy Templates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Spawning", meta = (AllowPrivateAccess = true))
	class UDataTable* EnemyPresetsDataTable;

	// The Curve Table associated with difficulty scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Spawning", meta = (AllowPrivateAccess = true))
	class UCurveTable* DifficultyCurveTable;

	// The Class for the Default Gameplay Effect that applies stats to the enemies, to be instanced when needed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Spawning", meta = (AllowPrivateAccess = true))
	TSubclassOf<class UGameplayEffect> DefaultEffectClass;

private:
	class AEnemyBase* SpawnEnemy(const struct FEnemyTemplateTableRow& EnemyTemplate, const FVector& SpawnLocation, float MaxGroupSize, TMap<FName, FRealCurve*> DifficultyTableRows, AAttackGroup* NewAttackGroup);

};
