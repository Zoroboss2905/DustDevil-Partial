// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackGroup.generated.h"

enum EEnemyType : uint8;

UCLASS()
class DUSTDEVIL_API AAttackGroup : public AActor
{
	GENERATED_BODY()

public:
	// A number representing the maximum unit cost of this group.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackGroup", meta = (ExposeOnSpawn = true))
	float RemainingGroupCost;

	// The natural Detection Radius of the Attack Group. When the player enters, this alerts the entire group.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackGroup", meta = (ExposeOnSpawn = true))
	float DetectionRadius = 1000.f;

	// Whether this AttackGroup has been Triggered or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackGroup")
	bool bTriggered = false;
	
	// The natural Detection Radius of the Attack Group. When the player enters, this alerts the entire group.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackGroup", meta = (AllowPrivateAccess=true, Bitmask="EnemyTypes", BitmaskEnum = "/Script/DustDevil.EEnemyTypes"))
	int32 FilterWhitelist;
	
public:	
	// Sets default values for this actor's properties
	AAttackGroup();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DrawDebugSymbols(float DeltaTime);
	
	// Add an enemy to this attack Group
	bool AddEnemyToGroup(class AEnemyBase* NewEnemy);

	// Set the Enemy Manager
	void SetManager(class AEnemyManager* NewManager);

	// Remove an Enemy from the Group, Typically due to the enemy being killed or dying
	UFUNCTION()
	void RemoveEnemyFromGroup(class AEnemyBase* EnemyToRemove);

	UFUNCTION(BlueprintCallable, Category = "AttackGroup|Management")
	bool HasMembers() const;
	
public:
	// This Function is called every time an enemy is removed from the AttackGroup. This is typically due to the enemy being killed.
	UFUNCTION(BlueprintImplementableEvent, Category = "AttackGroup|Management")
	void OnEnemyRemoved();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// A List of all members within the attack group
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackGroup", meta = (AllowPrivateAccess = true))
	TArray<class AEnemyBase*> Members;

	// A Worldspace representation of the average location between members
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackGroup", meta = (AllowPrivateAccess = true))
	FVector AverageMemberLocation;

	// The Maximum Distance at which the TargetLocation can be, from enemy group to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackGroup", meta = (AllowPrivateAccess = true))
	float MaximumGroupPathingDistance = 10000.f;
private:
	// The TargetLocation of the AttackGroup, namely a position directly towards the player at a maximum distance from its members
	FVector TargetLocation;

	// Timer Handle for the Location Updater
	FTimerHandle LocationTimerHandle;

	// The Owning Enemy Manager, for Binding a Delegate in BeginPlay()
	class AEnemyManager* OwningManager;
	
private:
	void UpdateLocations();

};
