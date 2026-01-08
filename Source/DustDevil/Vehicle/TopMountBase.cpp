// Fill out your copyright notice in the Description page of Project Settings.


#include "TopMountBase.h"

#include "ProfilingDebugging/StallDetector.h"

// Sets default values
ATopMountBase::ATopMountBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup Turret mesh
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	// TurretMesh->SetupAttachment(RootComponent);
	RootComponent = TurretMesh;
}

void ATopMountBase::OverrideStats(float MaxCooldown, float MaxAmmo, float StartAmmo, float AmmoConsumption)
{
	CooldownMax = MaxCooldown;
	CooldownCurrent = 0.f;
	AmmunitionMax = MaxAmmo;
	AmmunitionCount = StartAmmo;
	AmmunitionConsumption = AmmoConsumption;
}

void ATopMountBase::InitWeaponStats(float FireRate, float MagSize, float ReloadTime)
{
	CooldownMax = 1.f / FMath::Max(FireRate, 0.000001);
	CooldownCurrent = 0.f;
	MagazineCapacity = MagSize;
	MagazineAmmoCount = MagSize;
	ReloadTimeMax = ReloadTime;
	ReloadTimeCurrent = 0.f;
}

void ATopMountBase::StartUse()
{
	bInputDown = true;
	TryFire(true);
	OnStartUse();
}

void ATopMountBase::OnStartUse_Implementation()
{
	// Do nothing, Implement in Blueprints
}

void ATopMountBase::TryFire(bool bStart)
{
	// Button Pressed
	if (bInputDown)
	{
		// Process potential Misfires
		if (MisfireCDCurrent <= 0.f)
		{
			// If weapon is Jammed
			if (DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_JAMMED))
			{
				// Jammed Gun takes priority
				MisfireCDCurrent = MisfireCDMax;
				Misfire(M_JAMMED);
				return;
			}
			// Weapon Mag is empty
			if ((DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_AMMO)))
			{
				// If the Player does not have enough ammunition left
				MisfireCDCurrent = MisfireCDMax;
				Misfire(M_AMMO);
				return;
			}
			// Weapon is on cooldown (and is being actively clicked, to prevent notif spam)
			if ((DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_COOLDOWN)) && bStart)
			{
				// When weapon has a decent amount of Cooldown time left and the player attempts to fire the weapon again
				MisfireCDCurrent = MisfireCDMax;
				Misfire(M_COOLDOWN);
				return;
			}
		}

		// If Weapon has no Disabled Reasons
		if (DisabledReasons == 0 || (bReloadCancellable && DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING)))
		{
			// Call BP Fire function
			Fire();
			// Start Cooldown timer
			CooldownCurrent = CooldownMax;
		}
	}
}

void ATopMountBase::StopUse()
{
	bInputDown = false;
	OnStopUse();
}

void ATopMountBase::TryReload(EReloadType ReloadType)
{
	// If already reloading, don't try to start reloading
	if (DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING))
	{
		TryReloadFailed(EReloadFailureReason::RF_RELOADING);
		return;
	}

	// If ammunition is full, don't try to start reloading
	if (MagazineAmmoCount == MagazineCapacity)
	{
		TryReloadFailed(EReloadFailureReason::RF_AMMOFULL);
		return;
	}
	
	// Call Blueprint OnTryReload for extra visual/audio after c++ checks.
	OnTryReload(ReloadType);
	// Start Reloading
	ReloadStarted();
}

void ATopMountBase::CancelReload()
{
	// If already reloading, try to cancel reloading
	if (DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING))
	{
		// Call TryReloadFailed
		TryReloadFailed(EReloadFailureReason::RF_CANCELLED);

		// Set Reload information back to default.
		// Reset Time Current back to 0.f
		ReloadTimeCurrent = 0.f;
		// Reset ReloadSpeed Multiplier back to 1.f
		ReloadSpeedMulti = 1.f;
		// Remove Reloading Flag
		DisabledReasons &= ~static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING);
	}
}

void ATopMountBase::OnReloadStarted_Implementation()
{
	// Do nothing, implement in blueprints
}

void ATopMountBase::OnTryReload_Implementation(EReloadType ReloadType)
{
	// Do nothing, implement in blueprints
}

void ATopMountBase::OnStopUse_Implementation()
{
	// Do nothing, implement in blueprints
}

// Called when the game starts or when spawned
void ATopMountBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATopMountBase::ReloadStarted()
{
	// Add the Reloading Flag to the Weapon
	DisabledReasons |= static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING);

	// Reset Reload timer the start
	ReloadTimeCurrent = 0.f;
	
	// Call Blueprint Version of Function
	OnReloadStarted();
}

void ATopMountBase::ReloadFinished()
{
	// Remove the Reloading Flag form the weapon
	DisabledReasons &= ~static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING);

	// Call Blueprint Version of Function
	OnReloadFinished();
}

void ATopMountBase::TryReloadFailed(EReloadFailureReason FailureReason)
{
	// Call Blueprint Version of Function
	OnTryReloadFailed(FailureReason);
}

void ATopMountBase::Aim(float NewAngle)
{
	// Input is managed prior to this function, just rotate the mesh to match the angle provided
	// If the turret should not aim, do not aim
	if (!bAimable)
	{
		return;
	}

	// If the Turret has an Owner, Get the Quat of the owner and adapt the rotation.
	if(IsValid(Owner))
	{
		// The Owner (Usually Vehicle) rotation
		FQuat OwnerQuat = Owner->GetActorQuat();

		// Subtract the Owner's Yaw Rotation (put yaw rotation in world space)
		FQuat CounterRotation = FRotator(0.f, -Owner->GetActorRotation().Yaw, 0.f).Quaternion();
		// Add aim rotation
		FQuat MoreRotation = FRotator(0.f, NewAngle, 0.f).Quaternion();

		// Set Turret Rotation
		TurretMesh->SetWorldRotation(OwnerQuat * CounterRotation * MoreRotation);
	}
	else
	{
		// Set Turret Rotation
		TurretMesh->SetWorldRotation(FRotator(0.f, NewAngle, 0.f));
	}
}

UStaticMeshComponent* ATopMountBase::GetTurretMesh()
{
	return TurretMesh;
}

void ATopMountBase::OnReloadFinished_Implementation()
{
	// Do nothing, implement in blueprints
}

void ATopMountBase::OnTryReloadFailed_Implementation(EReloadFailureReason FailureReason)
{
	// Do nothing, implement in blueprints
}

// Called every frame
void ATopMountBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Handle multiple bullets being spawned per tick
	int ExtraFireCounter = 0;
	
	// Decrement Cooldown
	if (CooldownCurrent > 0.f)
	{
		CooldownCurrent -= DeltaTime;
		// If it doesn't have Cooldown flag, add it 
		if (!(DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_COOLDOWN)))
		{
			DisabledReasons |= static_cast<int32>(EWeaponDisabledReasons::WDR_COOLDOWN);
		}
	}
	else
	{
		while (CooldownCurrent < -FMath::Max(CooldownMax, 0.0001f))
		{
			ExtraFireCounter++;
			CooldownCurrent += CooldownMax;
		}
		CooldownCurrent = 0.f;
		// If it has Cooldown flag, remove it 
		if (DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_COOLDOWN))
		{
			DisabledReasons &= ~static_cast<int32>(EWeaponDisabledReasons::WDR_COOLDOWN);
		}
	}
	
	// Decrement Misfire Cooldown
	if (MisfireCDCurrent > 0.f)
	{
		MisfireCDCurrent -= DeltaTime;
	}

	// Reloading 
	if (DisabledReasons & static_cast<int32>(EWeaponDisabledReasons::WDR_RELOADING))
	{
		// Increment Reload Timer
		ReloadTimeCurrent += DeltaTime * ReloadSpeedMulti;
		// If reload time has been reached
		if (ReloadTimeCurrent >= ReloadTimeMax)
		{
			// Reset Time Current back to 0.f
			ReloadTimeCurrent = 0.f;
			// Reset ReloadSpeed Multiplier back to 1.f
			ReloadSpeedMulti = 1.f;
			ReloadFinished();
		}
	}
	
	// Attempt to fire (passively, not actively)
	TryFire(false);
	// If extra bullets need to spawn, spawn more bullets
	while (ExtraFireCounter > 0)
	{
		TryFire(false);
		ExtraFireCounter--;
	}
}

