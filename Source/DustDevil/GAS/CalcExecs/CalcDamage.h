// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CalcDamage.generated.h"

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API UCalcDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UCalcDamage();

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
