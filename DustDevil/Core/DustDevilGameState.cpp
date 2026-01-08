// Fill out your copyright notice in the Description page of Project Settings.


#include "DustDevilGameState.h"

void ADustDevilGameState::Publish_EnemyDeath(AEnemyBase* Enemy)
{
	// Broadcast the OnEnemyDeath Delegate, with the specific Enemy that is dying
	OnDeathDelegate.Broadcast(Enemy);
}

void ADustDevilGameState::Publish_AttackGroupDissolved(AAttackGroup* AttackGroup)
{
	// Broadcast the AttackGroupDissolved Delegate, with the specific AttackGroup that is dying
	OnAttackGroupDissolvedDelegate.Broadcast(AttackGroup);
}