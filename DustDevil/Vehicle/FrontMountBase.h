// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrontMountBase.generated.h"

UCLASS()
class DUSTDEVIL_API AFrontMountBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFrontMountBase();

	// CollisionBoxExtents is the HALF-Size of the Box, with the CollisionBoxOffset as the Center
	UFUNCTION(BlueprintCallable, Category = "FrontMount")
	void Setup(float NewDamage, const FVector& CollisionBoxExtents, const FVector& CollisionBoxOffset);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FrontMount", meta = (AllowPrivateAccess = true))
	class UBoxComponent* CollisionBox;
	float Damage;
};
