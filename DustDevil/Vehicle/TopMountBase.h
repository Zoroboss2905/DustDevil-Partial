// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopMountBase.generated.h"

UENUM(BlueprintType)
enum EMisfireType
{
	M_AMMO		UMETA(DisplayName = "Not Enough Ammo"),
	M_COOLDOWN	UMETA(DisplayName = "Weapon on Cooldown"),
	M_JAMMED		UMETA(DisplayName = "Weapon Jammed"),
	M_UNKNOWN		UMETA(DisplayName = "Unknown")
};

UENUM(BlueprintType)
enum EReloadType
{
	R_MANUAL		UMETA(DisplayName = "Manual",	ToolTip = "Reload Triggered Manually by the player."),
	R_EMPTY		UMETA(DisplayName = "Empty",	ToolTip = "Automatic, Magazine Empty"),
	R_FORCED		UMETA(DisplayName = "Forced",	ToolTip = "Automatic, Forced by some event in blueprint")
};

UENUM(BlueprintType)
enum EReloadFailureReason
{
	RF_UNKNOWN			UMETA(DisplayName = "Unknown", ToolTip = "ReloadFailureReason not correctly Defined"),
	RF_AMMOFULL		UMETA(DisplayName = "Ammo Full", ToolTip = "Magazine/Belt is already full"),
	RF_IRRELEVANT		UMETA(DisplayName = "Irrelevant", ToolTip = "This weapon does not need to reload"),
	RF_CANCELLED		UMETA(DisplayName = "Cancelled", ToolTip = "Reload was cancelled"),
	RF_RELOADING		UMETA(DisplayName = "Reloading", ToolTip = "This weapon is still being reloaded")
};

// A bitmasked enumerator for many reasons the weapon may not be able to fire
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EWeaponDisabledReasons : uint8
{
	WDR_NONE = 0				UMETA(DisplayName = "None",			ToolTip = "Weapon can be used"),
	WDR_JAMMED = 1 << 0			UMETA(DisplayName = "Jammed",		ToolTip = "Weapon is Jammed"),
	WDR_RELOADING = 1 << 1		UMETA(DisplayName = "Reloading",	ToolTip = "Weapon is Jammed"),
	WDR_SAFE = 1 << 2			UMETA(DisplayName = "Safe Zone",	ToolTip = "Weapon is within a Safe Zone"),
	WDR_AMMO = 1 << 3			UMETA(DisplayName = "Out of Ammo",	ToolTip = "Weapon has no ammunition in the Magazine"),
	WDR_COOLDOWN = 1 << 4		UMETA(DisplayName = "Cooldown",		ToolTip = "Weapon is on Cooldown")
};
ENUM_CLASS_FLAGS(EWeaponDisabledReasons)

UCLASS()
class DUSTDEVIL_API ATopMountBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATopMountBase();

	UFUNCTION(BlueprintCallable, Category = "TopMount", meta = (DeprecatedFunction))
	void OverrideStats(float MaxCooldown = 0.5f, float MaxAmmo = 10, float StartAmmo = 10, float AmmoConsumption = 1.f);

	/* Initialise all Stats for this weapon
	The Weapon's Stats and their expected format are as follows:
	FireRate: Represents the rate at which the weapon calls the Fire() function in number of calls per second
	MagSize: The number of bullets available in the Magazine/belt of the weapon, as a whole number where applicable
	ReloadTime: The Amount of Time in Seconds that it takes to reload this weapon.
	 */
	UFUNCTION(BlueprintCallable, Category = "TopMount|Setup")
	void InitWeaponStats(float FireRate = 1.f, float MagSize = 1.f, float ReloadTime = 1.f);
	
	// Activate the weapon
	UFUNCTION(BlueprintCallable, Category = "TopMount")
	void StartUse();

	// A Blueprint function called when the weapon starts being used (successful or not)
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount")
	void OnStartUse();

	// Attempt to use the weapon
	void TryFire(bool bStart);

	// Successfully Fire the weapon.
	UFUNCTION(BlueprintImplementableEvent, Category = "TopMount")
	void Fire();

	// Fail to Fire the weapon.
	UFUNCTION(BlueprintImplementableEvent, Category = "TopMount")
	void Misfire(EMisfireType MisfireReason);

	// Deactivate the weapon
	UFUNCTION(BlueprintCallable, Category = "TopMount")
	void StopUse();

	// A Blueprint function called when the weapon is no longer being used
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount")
	void OnStopUse();

	// Reloading Functions

	// Call this function to Attempt to reload the weapon. Alternate functionality can be implemented based on how the reload was called
	UFUNCTION(BlueprintCallable, Category = "TopMount|Reloading")
	void TryReload(EReloadType ReloadType);
	UFUNCTION(BlueprintCallable, Category = "TopMount|Reloading")
	void CancelReload();
	
	// Blueprint Event called when attempting to reload. (Occurs Regardless of Success or Failure )
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount|Reloading")
	void OnTryReload(EReloadType ReloadType);
	// Blueprint Event called when the weapon successfully begins reloading
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount|Reloading")
	void OnReloadStarted();
	// Blueprint Event called when the weapon Successfully finishes reloading
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount|Reloading")
	void OnReloadFinished();
	// Blueprint Event called when the weapon Fails to reload
	UFUNCTION(BlueprintNativeEvent, Category = "TopMount|Reloading")
	void OnTryReloadFailed(EReloadFailureReason FailureReason);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Use the Aim input to point this Object towards the Cursor (or in the direction of the controller input)
	void Aim(float NewAngle);

public:
	// Getter Function for the Turret's mesh
	UStaticMeshComponent* GetTurretMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Local Functions for Reloading the weapon.
	void ReloadStarted();
	void ReloadFinished();
	void TryReloadFailed(EReloadFailureReason FailureReason);

private:	// Structure
	// The Mesh for the Turret, rotates alongside the AimArrow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopMount|Mesh", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* TurretMesh;

private:	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Ammunition", meta = (AllowPrivateAccess = true, DeprecatedProperty))
	float AmmunitionCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Ammunition", meta = (AllowPrivateAccess = true, DeprecatedProperty))
	float AmmunitionConsumption;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Ammunition", meta = (AllowPrivateAccess = true, DeprecatedProperty))
	float AmmunitionMax;

	// Magazine

	// The Size of the Magazine of the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Ammunition", meta = (AllowPrivateAccess = true))
	float MagazineCapacity;
	// The Current Ammo Count stored in the Magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Ammunition", meta = (AllowPrivateAccess = true))
	float MagazineAmmoCount;

	// Reloading

	// The Time it takes the weapon to Reload its Magazine/Belt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Reload", meta = (AllowPrivateAccess = true))
	float ReloadTimeMax;
	// The amount of time that the weapon has been reloading for. (0 - ReloadTimeMax)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Reload", meta = (AllowPrivateAccess = true))
	float ReloadTimeCurrent;
	// A Multiplier to the Rate at which the weapon Reloads, useful for 'Perfect Reloads'.
	// Resets to 1.f when reload is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Reload", meta = (AllowPrivateAccess = true))
	float ReloadSpeedMulti = 1.f;

	// Should the Turret Rotate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function", meta = (AllowPrivateAccess=true))
	bool bAimable;
	// Should the weapon reload automatically
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function", meta = (AllowPrivateAccess=true))
	bool bAutoReload = true;
	// Should the user be able to try and Cancel the Reload?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function", meta = (AllowPrivateAccess=true))
	bool bReloadCancellable = false;
	/* A Bitmask of the various reasons a weapon may be disabled
	 * Additively:
	 * 0 = None
	 * 1 = Jammed
	 * 2 = Reloading
	 * 4 = Safe Zone
	 * 8 = Out of Ammo
	 * 16 = Cooldown
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function", meta = (AllowPrivateAccess=true, Bitmask="", BitmaskEnum = "/Script/DustDevil.EWeaponDisabledReasons"))
	int32 DisabledReasons = 0;
	// Is the weapon Fire button being held down
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopMount|Function", meta = (AllowPrivateAccess=true))
	bool bInputDown;
	// The Cooldown time between the weapon Firing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopMount|Function|Cooldowns", meta = (AllowPrivateAccess=true))
	float CooldownMax;
	// The amount of time (in seconds) before the weapon can fire again
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopMount|Function|Cooldowns", meta = (AllowPrivateAccess=true))
	float CooldownCurrent;
	// The Amount of time remaining until the weapon can call Misfire()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopMount|Function|Cooldowns", meta = (AllowPrivateAccess=true))
	float MisfireCDCurrent;
	// The amount of time (in seconds) between Misfire calls
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TopMount|Function|Cooldowns", meta = (AllowPrivateAccess=true))
	float MisfireCDMax = 1.f;
};

