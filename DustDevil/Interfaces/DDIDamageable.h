// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DustDevil/DustDevilCommonData.h"
#include "DDIDamageable.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDDIDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUSTDEVIL_API IDDIDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Get the Type of this object, Typically an Enemy, however can be applied to other objects too
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	UPARAM(meta = (Bitmask = "EnemyTypes", BitmaskEnum = "/Script/DustDevil.EEnemyTypes")) int32 GetDamageableType();
};
