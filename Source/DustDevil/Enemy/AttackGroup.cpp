// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackGroup.h"

#include "EnemyBase.h"
#include "EnemyManager.h"
#include "DustDevil/DustDevilCommonData.h"
#include "DustDevil/Core/DustDevilGameState.h"
#include "DustDevil/Interfaces/DDEnemyEventManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAttackGroup::AAttackGroup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void AAttackGroup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Interpolate current position towards the TargetLocation
	SetActorLocation(FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime*4.f));
}

void AAttackGroup::DrawDebugSymbols(float DeltaTime)
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), 256, 16, FColor::Red, false, DeltaTime*1.5f);
	DrawDebugSphere(GetWorld(), TargetLocation, 128, 16, FColor::Purple, false, DeltaTime*1.5f);
	DrawDebugSphere(GetWorld(), AverageMemberLocation, 128, 16, FColor::Blue, false, DeltaTime*1.5f);

	for (AEnemyBase* Member : Members)
	{
		Member->DrawDebugSymbols(DeltaTime);
	}
}

bool AAttackGroup::AddEnemyToGroup(class AEnemyBase* NewEnemy)
{
	// Add enemy to Member's List
	Members.Add(NewEnemy);
	// Set its owning attack group to this
	NewEnemy->SetOwningAttackGroup(this);
	return true;
}

void AAttackGroup::SetManager(class AEnemyManager* NewManager)
{
	OwningManager = NewManager;
}

void AAttackGroup::RemoveEnemyFromGroup(AEnemyBase* EnemyToRemove)
{
	// If the parsed Enemy is not Valid, return early
	if (!EnemyToRemove)
	{
		return;
	}

	// And Exists in the Members List
	if (Members.Contains(EnemyToRemove))
	{
		// Remove it from the members list
		Members.Remove(EnemyToRemove);
		
		// Call the BP Implementation of the function
		OnEnemyRemoved();
	}

	// Then check the Number of enemies remaining
	if (Members.Num() == 0)
	{
		// Broadcast Delegate for Group Dissolving
		if (IDDEnemyEventManager* DDGameState = Cast<IDDEnemyEventManager>(GetWorld()->GetGameState()))
		{
			// If LocationTimerHandle is valid and/or active
			if (LocationTimerHandle.IsValid())
			{
				// Invalidate the handle
				LocationTimerHandle.Invalidate();
			}
			// Publish AttackGroupDissolved.
			DDGameState->Publish_AttackGroupDissolved(this);
		}
	}
}

bool AAttackGroup::HasMembers() const
{
	return !Members.IsEmpty();
}

// Called when the game starts or when spawned
void AAttackGroup::BeginPlay()
{
	Super::BeginPlay();

	// Set all Average and TargetLocations to the AttackGroup Spawn Location
	FVector SpawnLocation = GetActorLocation();
	AverageMemberLocation = SpawnLocation;
	TargetLocation = SpawnLocation;

	// Bind to Dissolving Delegate
	if (ADustDevilGameState* DDGameState = Cast<ADustDevilGameState>(GetWorld()->GetGameState()))
	{
		if (OwningManager)
		{
			DDGameState->OnAttackGroupDissolvedDelegate.AddDynamic(OwningManager, &AEnemyManager::DissolveAttackGroup);
		}
	}
	
	// Start a Timer for updating all Location information
	// Update every second
	GetWorld()->GetTimerManager().SetTimer(LocationTimerHandle, this, &AAttackGroup::UpdateLocations, 0.1f, true);
}

void AAttackGroup::UpdateLocations()
{
	// Update both the AverageMemberPosition and the Position of the AttackGroup itself, based on distance to the player
	// (This exists to minimise pathfinding requirements, enemies path to the AttackGroup location, AttackGroupLocation is in the direction of the Player, but a set distance from the group of enemies)
	
	FVector TotalPositions = FVector(0);
	for (AEnemyBase* Enemy : Members)
	{
		if (!IsValid(Enemy))
		{
			UE_LOG(DDLog, Warning, TEXT("[AttackGroup]|[UpdateLocations]: Enemy Was Invalid!"))
		}
		
		TotalPositions += Enemy->GetActorLocation();
	}

	// Ternary operators to prevent dividing by 0;
	AverageMemberLocation = FVector(TotalPositions.X / (Members.IsEmpty() ? 1 : Members.Num()),
									TotalPositions.Y / (Members.IsEmpty() ? 1 : Members.Num()),
									TotalPositions.Z / (Members.IsEmpty() ? 1 : Members.Num()));

	// Then draw a Line from the AverageMemberLocation to the Player.
	// Get the Player's Location
	FVector PlayerLocation = FVector(0);
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		PlayerLocation = PlayerPawn->GetActorLocation();
	}
	else
	{
		UE_LOG(DDLog, Error, TEXT("[AttackGroup]: Player Was Invalid, Could not Pathfind AttackGroup to Player"))
		return;
	}
	
	// Check the distance from the average member location to the player, and update the triggered status if within range
	if (FVector::Distance(PlayerLocation, AverageMemberLocation) < DetectionRadius)
	{
		bTriggered = true;
	}

	// If the AttackGroup is no triggered, end function early
	if (!bTriggered)
	{
		return;
	}
	
	// If the distance between the Members and the Player is greater than the Pathing Distance
	if (FVector::Distance(AverageMemberLocation, PlayerLocation) > MaximumGroupPathingDistance)
	{
		// Get Unit Vector in direction of the Player from the Members.
		FVector TempVector = (PlayerLocation - AverageMemberLocation);
		TempVector.Normalize();
		// Multiply that Vector by MaxGroupPathingDistance
		TempVector *= MaximumGroupPathingDistance;
		// Move that Location Up
		TempVector += FVector(0,0,1000) + AverageMemberLocation;
		// Draw a Line Directly to the Floor from there
		FHitResult TempHitResult;
		GetWorld()->LineTraceSingleByChannel(TempHitResult, TempVector, FVector(TempVector.X, TempVector.Y, TempVector.Z - 20000.f), ECC_WorldStatic);
		// Set the Target Location to that Location
		TargetLocation = TempHitResult.Location;
	}
	else
	{
		// Set the TargetLocation to the Player's Location
		TargetLocation = PlayerLocation;
	}
}

