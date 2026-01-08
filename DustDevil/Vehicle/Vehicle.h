// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "TopMountBase.h"
#include "Vehicle.generated.h"

enum class EHardwareDevicePrimaryType : uint8;
/**
 * 
 */
UCLASS()
class DUSTDEVIL_API AVehicle : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AVehicle();

public:
	// Setup Input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Overload Possession, for potential replication of GAS
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// Tick
	virtual void Tick(float DeltaSeconds) override;

	// Override Interface Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual class UVehicleAttributeSet* GetAttributeSet() const;

	// Input Detection and Switching

	// Triggers when the User's Input Changes
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle|Input")
	void OnInputChanged(EHardwareDevicePrimaryType NewDeviceType);

	// Mount Access and replacing
	// Replaces the Top Mount with a new one
	UFUNCTION(BlueprintCallable, Category="Vehicle|Mounts")
	void SetTopMount(class ATopMountBase* NewMount);

	UFUNCTION(BlueprintPure, Category="Vehicle|Mounts")
	class ATopMountBase* GetTopMount() const;

	// Blueprint function, bound to Acceleration Attribute Change
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|GAS")
	void OnAccelerationChanged(const float OldValue, const float NewValue);

	// Blueprint function, bound to Handling Attribute Change
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|GAS")
	void OnHandlingChanged(const float OldValue, const float NewValue);
	
	// Blueprint function, bound to TopSpeed Attribute Change
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|GAS")
	void OnTopSpeedChanged(const float OldValue, const float NewValue);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|Movement")
	void OnVehicleStartDrive();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|Movement")
	void OnVehicleStopDrive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|Movement")
	void OnVehicleBrakesApplied();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle|Movement")
	void OnVehicleBrakesReleased();
	
	// Getters
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return CameraBoom; }
	FORCEINLINE class USpringArmComponent* GetSweepSpringArm() const { return SweepCameraBoom; }
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* AccelerateInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* SteeringInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* BoostInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* BrakeInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* ShootInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* AimInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* PauseInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Input|Actions")
	class UInputAction* ReloadInputAction;

	// GAS
	UPROPERTY(VisibleAnywhere, Category = "Vehicle|Abilities")
	class UDD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Abilities")
	class UVehicleAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

protected:

	virtual void BeginPlay() override;

	// Ability System
	// Apply Default abilities
	void GiveDefaultAbilities();
	// Apply Default attributes
	void InitDefaultAttributes() const;

private:
	// Another Boom arm, for moving the Camera around laterally
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* SweepCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics", meta = (AllowPrivateAccess = true))
	class UBoxComponent* VehiclePhysObj;

	// Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* Chassis;

	// Wheel Sockets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics", meta = (AllowPrivateAccess = true))
	class USceneComponent* KnuckleFL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics", meta = (AllowPrivateAccess = true))
	class USceneComponent* KnuckleFR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics", meta = (AllowPrivateAccess = true))
	class USceneComponent* KnuckleBL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics", meta = (AllowPrivateAccess = true))
	class USceneComponent* KnuckleBR;

	// Wheel Meshes

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* WheelFL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* WheelFR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* WheelBL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* WheelBR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Meshes", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* EngineMount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Mounts", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* FrontMount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Mounts", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* RearMount;
	
	// The Top mount can be accessed with separate getters and setters
	UPROPERTY()
	class ATopMountBase* TopMount;

private:
	// TODO: Deprecate these values, as GAS now manages Rad Damage
	// Migrated from Blueprints
	// Vitals
	// Amount of Radiation Damage the player has taken.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vehicle|Vitals", meta = (AllowPrivateAccess = true, DeprecatedProperty))
	float Rads;
	// Movement
	// The Input value for Acceleration, used as a target for interpolating Acceleration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float AccelerationInput;
	// The Current Acceleration of the vehicle, Result of interpolating from AccelerationInput
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float Acceleration;
	
	// The Input Value for Steering, used as a target for interpolating steering and camera prediction.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float SteerInput;
	// The Current Speed the vehicle is turning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float SteerSpeed;
	// A Multiplier associated with the rate at which the vehicle turns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float SteeringAcceleration = 4; 

	// Curves for Vehicle Movement
	
	// The Acceleration Curve gives a Multiplier to the Acceleration of the vehicle based on the current speed of the vehicle (Total, not as a ratio)
	// For example: If the curve angles upwards, the vehicle accelerates more at higher speeds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	UCurveFloat* AccelerationCurve;
	// The Braking Curve gives a Multiplier to the Braking Power of the vehicle based on the current speed of the vehicle (Total, not as a ratio)
	// For example: If the curve angles upwards, the vehicle slows down much faster at higher speeds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	UCurveFloat* BrakingCurve;
	// The Steering Curve gives a Multiplier to the Handling of the vehicle based on the current speed of the vehicle (Total, not as a ratio)
	// For example: If the curve angles downwards, the vehicle turns slower at higher speeds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	UCurveFloat* SteeringCurve;
	
	// Tags for determining what state the vehicle is in.

	UPROPERTY(EditAnywhere, Category = "Vehicle|GameplayTags")
	FGameplayTag AcceleratingTag;
	UPROPERTY(EditAnywhere, Category = "Vehicle|GameplayTags")
	FGameplayTag BrakingTag;
	UPROPERTY(EditAnywhere, Category = "Vehicle|GameplayTags")
	FGameplayTag GroundedTag;
	UPROPERTY(EditAnywhere, Category = "Vehicle|GameplayTags")
	FGameplayTag ReversingTag;
	UPROPERTY(EditAnywhere, Category = "Vehicle|GameplayTags")
	FGameplayTag SteeringTag;

	// Is the Player using a controller?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Input", meta = (AllowPrivateAccess = true))
	bool bUsingController;

	// Camera
	// The Scale at which the Camera sweeps towards the User's Cursor (0 - 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	float CameraSweepAggression = 0.3f;
	// The Rate at which the Sweeping Camera moves to its target location (0 - 10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	float CameraSweepRate = 1.f;
	// Minimum Value for Camera distance (Zoom in and Out)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	float CameraDistanceMinimum = 2000.f;
	// Maximum Value for Camera distance (Zoom in and Out)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Camera", meta = (AllowPrivateAccess = true))
	float CameraDistanceMaximum = 8000.f;
	
	// The Location of the last CursorHitLocation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Cursor", meta = (AllowPrivateAccess = true))
	FVector CursorHitLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Movement", meta = (AllowPrivateAccess = true))
	float LateralSpeedLastTick = 0;
	
private:
	// C++ Exclusive (Not Exposed) Variables
	float SuspensionLength;
	float CurrentTopSpeed;
	bool bSteerDirty = false;
	bool bAccelDirty = false;
	
	// Camera Controls
	float LerpDirAngle;

	FTimerHandle SteeringEaseOutHandle;
	FTimerHandle AccelEaseOutHandle;
	FTimerHandle InputCheckingHandle;
private:

	// Internal Handling of Input Changed Delegate
	UFUNCTION()
	void HandleInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
	void CheckInput(const FPlatformUserId UserId);
	
	UFUNCTION()
	void EaseOutSteering(int CallCount);
	UFUNCTION()
	void EaseOutAccel(int CallCount);

private:
	// Mesh Update Functions (Visual)
	void UpdateWheelRotation(float DeltaTime, float CurrentMoveSpeed) const;
	// Cause suspension on the vehicle, allowing for slope traversal
	void DoSuspension(float DeltaTime);
	// Clamp Pitch (Forward/Backward) and Roll (Left/Right) Rotation of the vehicle
	void LockVehicleRotation() const;
	// Clamp the Velocity of the Vehicle
	void DoSpeedLimit(float DeltaTime);
	// Update camera Panning
	void MoveCamera(float DeltaTime);

	// Movement Functions
	void Accelerate(const struct FInputActionValue& Value);
	void StartAccelerating();
	void StopAccelerating();
	void Steer(const struct FInputActionValue& Value);
	void StartSteering();
	void StopSteering();
	void StartBraking();
	void StopBraking();
	void BoostPressed();
	void BoostReleased();

	// Weapon Extensions
	void StartTopMount();
	void ManualTryReloadTopMount();
	void TryReloadTopMount(EReloadType ReloadType = EReloadType::R_MANUAL) const;
	void AimController(const struct FInputActionValue& Value);
	void AimMouse() const;
	void StopTopMount();

	// Ability System
	void InitAbilitySystemComponent();

	// Attribute Change Delegates

	// Delegate to handle the change in the Acceleration attribute
	virtual void OnAccelerationAttributeChanged(const struct FOnAttributeChangeData& Data);
	// Delegate to handle the change in the Handling attribute
	virtual void OnHandlingAttributeChanged(const struct FOnAttributeChangeData& Data);
	// Delegate to handle the change in the TopSpeed attribute
	virtual void OnTopSpeedAttributeChanged(const struct FOnAttributeChangeData& Data);
};


