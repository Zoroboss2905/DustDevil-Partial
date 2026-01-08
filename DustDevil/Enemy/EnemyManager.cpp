// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include "AttackGroup.h"
#include "../DustDevilCommonData.h"
#include "EnemyBase.h"
#include "GameplayEffect.h"
#include "NavigationSystem.h"
#include "DustDevil/Core/DustDevilGameState.h"
#include "DustDevil/Interfaces/DDEnemyEventManager.h"
#include "Engine/CurveTable.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// If EnemyPresetsDataTable not set
	if (!EnemyPresetsDataTable)
	{
		// Get Enemy Data Table from filepath
		static ConstructorHelpers::FObjectFinder<UDataTable> DataTableObj(TEXT("/Script/Engine.DataTable'/Game/DustDevil/Characters/Enemies/DT_EnemyTemplates.DT_EnemyTemplates'"));
		if (DataTableObj.Succeeded())
		{
			// Assign object
			EnemyPresetsDataTable = DataTableObj.Object;
			UE_LOG(DDLog, Log, TEXT("EnemyManager: Enemy Presets Data Table Set successfully."));
		}
		else
		{
			// Couldn't assign object, notify Developer
			UE_LOG(DDLog, Error, TEXT("EnemyManager: Enemy Presets Data Table was not found! Please Set Manually!"));
		}
	}
	else
	{
		// If data table is already set, no need to reassign.
		UE_LOG(DDLog, Log, TEXT("EnemyManager: Enemy Presets Data Table already Set, Skipping Automatic Reassignment."));
	}

	// If DifficultyCurveTable not set
	if (!DifficultyCurveTable)
	{
		// Get the Difficulty Curve Table
		static ConstructorHelpers::FObjectFinder<UCurveTable> CurveTableObj(TEXT("/Script/Engine.CurveTable'/Game/DustDevil/Characters/Enemies/CT_Difficulty.CT_Difficulty'"));
		if (CurveTableObj.Succeeded())
		{
			// Assign object
			DifficultyCurveTable = CurveTableObj.Object;
			UE_LOG(DDLog, Log, TEXT("EnemyManager: Difficulty Curve Table Set successfully."));
		}
		else
		{
			// Couldn't assign object, notify Developer
			UE_LOG(DDLog, Error, TEXT("EnemyManager: Difficulty Curve Table was not found! Please Set Manually!"));
		}
	}
	else
	{
		UE_LOG(DDLog, Log, TEXT("EnemyManager: Difficulty Curve Table already Set, Skipping Automatic Reassignment."));
	}	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	// Draw some Debug Spheres
#if WITH_EDITOR
	if (bDebugSymbols)
	{
		for (AAttackGroup* Group : AttackGroups)
		{
			Group->DrawDebugSymbols(DeltaTime);
		}
	}
#endif
}

void AEnemyManager::CreateAttackGroup(int32 Whitelist, FVector SpawnLocation, TArray<struct FEnemyTemplateTableRow> PresetGroup,
	TSubclassOf<AAttackGroup> CustomAttackGroupClass, AAttackGroup*& NewAttackGroup)
{
	/* Method:
	The CreateAttackGroup function has two main modes, entirely dictated by whether a PresetGroup is supplied.
	If the preset group has ANYTHING in it:
		Spawn Explicitly each thing inside this list and nothing else.
		Scale the stats of the enemies based on the GlobalDifficulty.
		Do not care about any traditional exclusions, including GroupCost, EnemyTier or Whitelist
	If the Preset Group is EMPTY:
		If Whitelist is Empty:
			Randomise whitelist
		Spawn a Random Difficulty Scaled enemy from the TemplateList (with exclusions for BOSS or SPECIAL enemies) that is within budget
		Add enemy to Attack Group
		Repeat until either Full, or getting a random enemy fails 20 times in a row.
		
	 */

	// Get AttackGroup Subclass if supplied
	UClass* NewAttackGroupClass = AAttackGroup::StaticClass();
	if (CustomAttackGroupClass)
	{
		NewAttackGroupClass = CustomAttackGroupClass;
	}
	
	// Setup Output Group
	FActorSpawnParameters SpawnParams;
	// NewAttackGroup = GetWorld()->SpawnActor<AAttackGroup>(NewAttackGroupClass, SpawnLocation, FRotator(0), SpawnParams);
	FTransform NewAttackGroupTransform;
	NewAttackGroupTransform.SetLocation(SpawnLocation);
	NewAttackGroup = GetWorld()->SpawnActorDeferred<AAttackGroup>(NewAttackGroupClass, NewAttackGroupTransform);
	// Set its manager
	NewAttackGroup->SetManager(this);
	NewAttackGroup->FinishSpawning(NewAttackGroupTransform);
#pragma region Difficulty Scaling
	// Difficulty Scaled Values, Speed, Health, ChitinChance, GroupSize
	// If for some reason the Difficulty Curve table is invalid, return early.
	if (!IsValid(DifficultyCurveTable))
	{
		UE_LOG(DDLog, Error, TEXT("Difficulty Curve Table Not Set, No Enemies can spawn."));
		return;
	}
	float MaxGroupSize = 10.f;
	
	// Get the TableRows from the table
	TMap<FName, FRealCurve*> DifficultyTableRows = DifficultyCurveTable->GetRowMap();

	if (FRealCurve* GroupSizeCurve = DifficultyTableRows.FindRef(FName("EnemyGroupSize")))
	{
		MaxGroupSize = GroupSizeCurve->Eval(GlobalDifficulty);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Enemy Group Size Not Found in %s!"), *DifficultyCurveTable->GetName())
	}

#pragma endregion
	
	// If the Preset Group has ANYTHING in it
	if (!PresetGroup.IsEmpty())
	{
		// Spawn everything inside this list and nothing else
		for (FEnemyTemplateTableRow EnemyTemplate : PresetGroup)
		{
			
			// All the Data within the Template
			/*
			Enemy Sub Class
			Base Stats Struct
				Health
				Armour
				Damage
				BaseSpeed
				BaseBioMatter
				ChitinDropChance
			Scale
			CostValue
			Tier
			EnemyTypes
			Starting GAS Abilities
			Starting GAS Effects
			 */

			// Spawn New Enemy
			AEnemyBase* NewEnemy = SpawnEnemy(EnemyTemplate, SpawnLocation, MaxGroupSize, DifficultyTableRows, NewAttackGroup);
			
			// Add enemy to Attack Group
			NewAttackGroup->AddEnemyToGroup(NewEnemy);
		}
	}
	// The Preset Group was Empty
	else
	{
		// Basically the Same process as before, except first pick a random row and compare against some filters
		TArray<FEnemyTemplateTableRow*> AllPresets;
		EnemyPresetsDataTable->GetAllRows<FEnemyTemplateTableRow>(TEXT("DataTableAcquisitionContext"), AllPresets);
		
		// If the whitelist was not set, randomise the whitelist
		if (Whitelist == 0)
		{
			// All Filters enabled is 8, this is not a perfect random
			Whitelist = FMath::RandRange(1, 8);
		}

		NewAttackGroup->RemainingGroupCost = MaxGroupSize;
		
		// For Safety, a failed attempt counter, stops the loop if too many fails are hit
		int FailCountdown = 128;
		
		while (NewAttackGroup->RemainingGroupCost > 0.f && FailCountdown > 0)
		{
			// Safety check for the presets
			if (AllPresets.IsEmpty())
			{
				// Since we remove invalid options from the List as we go, we check the length here, exiting the loop when the list is empty
				break;
			}
			
			// First randomly get a row
			int RandIndex = FMath::RandRange(0, AllPresets.Num() - 1);
			FEnemyTemplateTableRow EnemyRow = *AllPresets[RandIndex];

			// Check Criteria
			if (EnemyRow.CostValue > NewAttackGroup->RemainingGroupCost ||								// Costs less than the remaining group cost
				EnemyRow.Tier == EEnemyTier::ET_BOSS || EnemyRow.Tier == EEnemyTier::ET_SPECIAL ||		// Not a 'Boss' or 'Special' Enemy
				!(EnemyRow.EnemyTypes & Whitelist) == EnemyRow.EnemyTypes)								// Does not have tags missing from the whitelist
			{
				// The Enemy did not meet the criteria
				// Remove it from the array, Decrement the FailCountdown and continue;
				AllPresets.RemoveAt(RandIndex);
				FailCountdown--;
				continue;
			}
			// If we hit this point, we can spawn the enemy in the row
			
			// Spawn New Enemy
			AEnemyBase* NewEnemy = SpawnEnemy(EnemyRow, SpawnLocation, MaxGroupSize, DifficultyTableRows, NewAttackGroup);

			// Decrement AttackGroup's remaining Cost
			NewAttackGroup->RemainingGroupCost -= EnemyRow.CostValue;
			
			// Add enemy to Group
			NewAttackGroup->AddEnemyToGroup(NewEnemy);
		}
	}
	// Add this new AttackGroup to the List of all AttackGroups.
	if (NewAttackGroup->HasMembers())
	{
		// Add to AttackGroups list	
		AttackGroups.Add(NewAttackGroup);
		NewAttackGroup->SetActorLocation(SpawnLocation);
	}
}

FEnemyTemplateTableRow AEnemyManager::GetEnemyPresetByName(FName TableRowName) const
{
	FEnemyTemplateTableRow NewTableRow;
	if (!EnemyPresetsDataTable)
	{
		UE_LOG(DDLog, Error, TEXT("EnemyManager: EnemyPresetDataTable Invalid! Cannot Get TableRow by Name!"));
		return NewTableRow;
	}

	NewTableRow = *EnemyPresetsDataTable->FindRow<FEnemyTemplateTableRow>(TableRowName, TEXT("GetPresetByNameContext"));

	return NewTableRow;
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	// If the DefaultEnemy GameplayEffect isn't Allocated, throw an Error at the user.
	if (!DefaultEffectClass)
	{
		// If this object is Derived from this specific class (it should be, to set the Gameplay effect)
		if (GetClass() != AEnemyManager::StaticClass())
		{
			UE_LOG(DDLog, Error, TEXT("EnemyManager: Default Enemy Stats GameplayEffect Invalid, Please Set Manually!"));
		}
	}
	else
	{
		UE_LOG(DDLog, Log, TEXT("EnemyManager: Default Enemy Stats GameplayEffect is Valid"));
	}
	
	// Old Begin Play
	/*
		Do Music stuff (ignore this entirely, this can be implemented in bp)

		Set Looping event for every 2 seconds			"Regroup All" 'AllocateEnemies'
		Set Looping function for every 1 seconds		"UpdateGroupAllocation"
		Set Looping function for every 0.05 seconds		"UpdateData"
		Set Looping function for every 0.5 seconds		"UpdateGroupData"
	*/

}

void AEnemyManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

AEnemyBase* AEnemyManager::SpawnEnemy(const struct FEnemyTemplateTableRow& EnemyTemplate,
	const FVector& SpawnLocation, float MaxGroupSize, TMap<FName, FRealCurve*> DifficultyTableRows,
	AAttackGroup* NewAttackGroup)
{
#pragma region DifficultyScaling
	// Apply Difficulty Coefficient
	// TODO: NuclearMaterialChance, AttackSpeed, DifficultyCoefficient
	float SpeedMulti = 1.f;
	float HealthMulti = 1.f;
	float NuclearChanceBonus = 1.f;
	float ChitinChanceBonus = 1.f;

	float DifficultyCoefficient = EnemyTemplate.DifficultyCoefficient;
	
	if (FRealCurve* SpeedCurve = DifficultyTableRows.FindRef(FName("EnemySpeedScale")))
	{
		SpeedMulti = SpeedCurve->Eval(GlobalDifficulty* DifficultyCoefficient);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Enemy Speed Scale Not Found in %s!"), *DifficultyCurveTable->GetName())
	}
	if (FRealCurve* HealthCurve = DifficultyTableRows.FindRef(FName("EnemyHealthScale")))
	{
		HealthMulti = HealthCurve->Eval(GlobalDifficulty * DifficultyCoefficient);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Enemy Group Size Not Found in %s!"), *DifficultyCurveTable->GetName())
	}
	if (FRealCurve* NuclearCurve = DifficultyTableRows.FindRef(FName("InnateNuclearChance")))
	{
		NuclearChanceBonus = NuclearCurve->Eval(GlobalDifficulty* DifficultyCoefficient);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Innate Nuclear Chance Not Found in %s!"), *DifficultyCurveTable->GetName())
	}
	if (FRealCurve* ChitinCurve = DifficultyTableRows.FindRef(FName("InnateChitinChance")))
	{
		ChitinChanceBonus = ChitinCurve->Eval(GlobalDifficulty* DifficultyCoefficient);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Innate Chitin Chance Not Found in %s!"), *DifficultyCurveTable->GetName())
	}
#pragma endregion
#pragma region RandomSpawnLocation(NavMesh)
	// Generate random location nearby to the SpawnLocation 
	FVector RandomSpawnLocation = SpawnLocation;
	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation RandomNavLocation = FNavLocation(FVector(0.f));
		float SearchRadius = FMath::Min(MaxGroupSize*100.f, 10000.f);
		if (NavSystem->GetRandomPointInNavigableRadius(SpawnLocation, SearchRadius, RandomNavLocation))
		{
			RandomSpawnLocation = RandomNavLocation;
		}
		else
		{
			UE_LOG(DDLog, Warning, TEXT("[%s] Navigation system failed to get Random Navigable Point, using backup method"), *GetName())
			// Attempt to get a random point on the Navmesh manually.
			// Generate random location within radius of SpawnLocation (Z goes up by 50)
			FVector RandomLineTraceStart = FVector(
				FMath::FRandRange(SpawnLocation.X - SearchRadius, SpawnLocation.X + SearchRadius),
				FMath::FRandRange(SpawnLocation.Y - SearchRadius, SpawnLocation.Y + SearchRadius),
				SpawnLocation.Z + 50.f
			);
			// Perform Line-trace to the ground, colliding with (NavMesh?)
			FHitResult IntermediateHit;
			GetWorld()->LineTraceSingleByChannel(IntermediateHit, RandomLineTraceStart, FVector(RandomLineTraceStart.X, RandomLineTraceStart.Y, RandomLineTraceStart.Z - 1000.f), ECC_WorldStatic);
			// If Line-trace hits anything
			if(IntermediateHit.bBlockingHit)
			{
				// Set RandomSpawnLocation to that point
				RandomSpawnLocation = IntermediateHit.Location;
			}
			else
			{
				UE_LOG(DDLog, Error, TEXT("Backup Location on NavMesh Failed to Hit Anything!"));
			}
		}
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("[%s] Navigation system failed to load"), *GetName());
	}
#pragma endregion
	// Spawn facing a random direction
	FRotator NewEnemyRotator = FRotator(0, FMath::FRand() * 360.f, 0);
	// Create Transform for Enemy Location rotation and Scale.
	// Scale is intended to increase alongside health after its above 20%
	FTransform NewEnemyTransform = FTransform(NewEnemyRotator, RandomSpawnLocation, FVector(FMath::Min(EnemyTemplate.Scale, HealthMulti * EnemyTemplate.Scale * 0.8f)));
	// Spawn subclass of Enemy
	UClass* NewActorClass = EnemyTemplate.EnemySubClass;
	// Spawn Actor, without running begin play or the BP Constructor
	AEnemyBase* NewEnemy = GetWorld()->SpawnActorDeferred<AEnemyBase>(NewActorClass, NewEnemyTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	// Add in the default attributes
	// If Default Effect Class is valid
	if (DefaultEffectClass)
	{
		// Apply Attributes to Enemy
		NewEnemy->ApplyInitialAttributes(DefaultEffectClass,
			EnemyTemplate.BaseStats.Health * HealthMulti,
			EnemyTemplate.BaseStats.Armour,
			EnemyTemplate.BaseStats.Damage,
			EnemyTemplate.BaseStats.BaseBioMatter,
			EnemyTemplate.BaseStats.NuclearMaterialChance + NuclearChanceBonus,
			EnemyTemplate.BaseStats.ChitinDropChance + ChitinChanceBonus);
	}
	else
	{
		UE_LOG(DDLog, Error, TEXT("DefaultEffectClass Invalid, Failed to apply Stats to Enemy."))
	}
	
	// Add Starting Effects and Abilities
	NewEnemy->DefaultEffects = EnemyTemplate.StartingEffects;
	NewEnemy->DefaultAbilities = EnemyTemplate.StartingAbilities;
	float NewSpeedVal = EnemyTemplate.BaseStats.BaseSpeed * SpeedMulti;
	NewEnemy->GetCharacterMovement()->MaxWalkSpeed = NewSpeedVal;
	NewEnemy->GetCharacterMovement()->MaxFlySpeed = NewSpeedVal;
	NewEnemy->GetCharacterMovement()->MaxSwimSpeed = NewSpeedVal;
	NewEnemy->TryAttackRange *= NewEnemyTransform.GetScale3D().X;

	// Set attack cooldown (account for divide by zero)
	NewEnemy->AttackCooldown = 1.f / (EnemyTemplate.BaseStats.AttackSpeed  > 0 ? EnemyTemplate.BaseStats.AttackSpeed : 0.0001f);

	// Spawn and Allocate an AI Controller
	NewEnemy->SpawnDefaultController();

	// Set ownership for the AttackGroup
	NewEnemy->SetOwningAttackGroup(NewAttackGroup);
	
	// Finalise the spawning of this enemy.
	NewEnemy->FinishSpawning(NewEnemyTransform);
	
	return NewEnemy;
}


void AEnemyManager::DissolveAttackGroup(class AAttackGroup* AttackGroup)
{
	if (!AttackGroup)
	{
		UE_LOG(DDLog, Warning, TEXT("[EnemyManager] AttackGroup was not Valid when DissolveAttackGroup was Called! Some things may not be working as intended!"));
		return;
	}

	// Safely Remove the Attack Group from the List of Attack Groups
	if (AttackGroups.Contains(AttackGroup))
	{
		AttackGroups.Remove(AttackGroup);
	}

	// Call BP Functionality (Effects and Objective Management)
	OnAttackGroupDissolved(AttackGroup);

	// Destroy AttackGroup
	AttackGroup->Destroy();
}
