// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustDevil/Interfaces/DDEnemyEventManager.h"
#include "GameFramework/GameStateBase.h"
#include "DustDevilGameState.generated.h"


UDELEGATE(Blueprintable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackGroupDissolvedSignature, AAttackGroup*, AttackGroup);

UDELEGATE(Blueprintable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AEnemyBase*, Enemy);


/**
 * 
 */
UCLASS()
class DUSTDEVIL_API ADustDevilGameState : public AGameStateBase, public IDDEnemyEventManager
{
	GENERATED_BODY()

public:
	// Delegates for Enemy Management

	// AttackGroups Dissolve and notify elsewhere using this delegate
	UPROPERTY(BlueprintAssignable, Category = "Enemy|Management")
	FAttackGroupDissolvedSignature OnAttackGroupDissolvedDelegate;
	// Enemies Die and notify elsewhere using this delegate
	UPROPERTY(BlueprintAssignable, Category = "Enemy|Management")
	FOnDeathSignature OnDeathDelegate;
		
public:
	UFUNCTION(BlueprintCallable, Category = "Enemy|Management")
	virtual void Publish_EnemyDeath(AEnemyBase* Enemy) override;
	UFUNCTION(BlueprintCallable, Category = "Enemy|Management")
	virtual void Publish_AttackGroupDissolved(AAttackGroup* AttackGroup) override;

};
