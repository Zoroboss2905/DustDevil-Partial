// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontMountBase.h"

#include "Components/BoxComponent.h"

// Sets default values
AFrontMountBase::AFrontMountBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
}

void AFrontMountBase::Setup(float NewDamage, const FVector& CollisionBoxExtents, const FVector& CollisionBoxOffset)
{
	Damage = NewDamage;

	// Apply new CollisionBox Extents and Position
	if (CollisionBox)
	{
		CollisionBox->SetRelativeLocation(CollisionBoxOffset);
		CollisionBox->SetBoxExtent(CollisionBoxExtents);
	}
}

// Called when the game starts or when spawned
void AFrontMountBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFrontMountBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

