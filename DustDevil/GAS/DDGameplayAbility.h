// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDGameplayAbility.generated.h"


UENUM(BlueprintType)
enum class EDDAbilityInputID : uint8
{
	// 0
	None = 0			UMETA(DisplayName = "None"),
	// 1
	Confirm				UMETA(DisplayName = "Confirm"),
	// 2
	Cancel				UMETA(DisplayName = "Cancel"),
	// 3
	Boost				UMETA(DisplayName = "Boost"),
	// 4
	SlowMotion			UMETA(DisplayName = "Slow Motion"),
	// 5
	Custom1				UMETA(DisplayName = "Custom1")
};

/**
 * 
 */
UCLASS()
class DUSTDEVIL_API UDDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Input")
	EDDAbilityInputID AbilityID;

	// A Name Associated with this Ability, should be IDENTICAL to the DT_Abilities table row name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FName AbilityName;
};
