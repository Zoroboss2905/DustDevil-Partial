// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "TopMountBase.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "DustDevil/DustDevilCommonData.h"
#include "DustDevil/Core/DustDevilPlayerState.h"
#include "DustDevil/GAS/VehicleAttributeSet.h"
#include "DustDevil/GAS/DDGameplayAbility.h"
#include "DustDevil/GAS/DD_AbilitySystemComponent.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AVehicle::AVehicle()
{
	PrimaryActorTick.bCanEverTick = true;
	/* Vehicle Hierarchy should look like:
	 * self
	 *	
	 *	VehiclePhysObj (Root Component) (Box)
	 *		Chassis
	 *			Mount_Top
	 *			Mount_Front
	 *			Mount_Engine
	 *			Wheel_FL
	 *			Wheel_FR
	 *			Wheel_BL
	 *			Wheel_BR
	 *		CameraViewLoc
	 *			CameraBoom
	 *				Camera
	 *---------------
	 * GAS Component
	 * 
	 */

	// Create Base Physics Object
	VehiclePhysObj = CreateDefaultSubobject<UBoxComponent>(TEXT("VehiclePhysicsObject"));
	SetRootComponent(VehiclePhysObj);

	// Build visual representation of the Vehicle
	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(VehiclePhysObj);

	// Mounted Parts
	
	FrontMount = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Front Mount"));
	FrontMount->SetupAttachment(Chassis, TEXT("Bumper_Socket"));
	
	RearMount = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Mount"));
	RearMount->SetupAttachment(Chassis, TEXT("Tank_Socket"));

	EngineMount = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Engine Mount"));
	EngineMount->SetupAttachment(Chassis, FName("Engine_Socket"));

	// Front Left Wheel & Knuckle
	KnuckleFL = CreateDefaultSubobject<USceneComponent>(TEXT("Knuckle_FL"));
	KnuckleFL->SetupAttachment(Chassis, FName("L_Wheel_Front_Socket"));
	WheelFL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel_FL"));
	WheelFL->SetupAttachment(KnuckleFL);
	// Front Right Wheel & Knuckle
	KnuckleFR = CreateDefaultSubobject<USceneComponent>(TEXT("Knuckle_FR"));
	KnuckleFR->SetupAttachment(Chassis, FName("R_Wheel_Front_Socket"));
	WheelFR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel_FR"));
	WheelFR->SetupAttachment(KnuckleFR);
	// Back Left Wheel & Knuckle
	KnuckleBL = CreateDefaultSubobject<USceneComponent>(TEXT("Knuckle_BL"));
	KnuckleBL->SetupAttachment(Chassis, FName("L_Wheel_Back_Socket"));
	WheelBL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel_BL"));
	WheelBL->SetupAttachment(KnuckleBL);
	// Back Right Wheel & Knuckle
	KnuckleBR = CreateDefaultSubobject<USceneComponent>(TEXT("Knuckle_BR"));
	KnuckleBR->SetupAttachment(Chassis, FName("R_Wheel_Back_Socket"));
	WheelBR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel_BR"));
	WheelBR->SetupAttachment(KnuckleBR);

	// Suspension Length = Front Wheel Radius
	if (UStaticMesh* Mesh = WheelFL->GetStaticMesh())
	{
		SuspensionLength = Mesh->GetBounds().SphereRadius;
	}

	// Setup Sweep Camera, Camera & Camera Boom
	SweepCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SweepArm"));
	SweepCameraBoom->SetupAttachment(VehiclePhysObj);
	SweepCameraBoom->TargetArmLength = 0.f;
	// Do Rotation and Collision
	SweepCameraBoom->bInheritPitch = false;
	SweepCameraBoom->bInheritYaw = true;
	SweepCameraBoom->bInheritRoll = false;
	SweepCameraBoom->bEnableCameraLag = true;
	SweepCameraBoom->CameraLagMaxDistance = 1000.f;
	SweepCameraBoom->CameraLagSpeed = 10.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(SweepCameraBoom);
	// Disable Rotation and Collision
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->CameraLagMaxDistance = 1000.0f;
	CameraBoom->CameraLagSpeed = 1.f;
	CameraBoom->SetRelativeRotation(FRotator(0.f, -60.f, 0.f));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	// GAS Component
	AbilitySystemComponent = CreateDefaultSubobject<UDD_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// Replication for multiplayer (if we use it later)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				// Bind Action(input action, trigger event, this, callback function)

				// Acceleration
				Input->BindAction(AccelerateInputAction, ETriggerEvent::Triggered, this, &AVehicle::Accelerate);
				Input->BindAction(AccelerateInputAction, ETriggerEvent::Started, this, &AVehicle::StartAccelerating);
				Input->BindAction(AccelerateInputAction, ETriggerEvent::Completed, this, &AVehicle::StopAccelerating);
				UE_LOG(DDLog, Log, TEXT("Bound Acceleration Input"))
				Input->BindAction(SteeringInputAction, ETriggerEvent::Triggered, this, &AVehicle::Steer);
				Input->BindAction(SteeringInputAction, ETriggerEvent::Started, this, &AVehicle::StartSteering);
				Input->BindAction(SteeringInputAction, ETriggerEvent::Completed, this, &AVehicle::StopSteering);
				UE_LOG(DDLog, Log, TEXT("Bound Steering Input"))
				Input->BindAction(BrakeInputAction, ETriggerEvent::Started, this, &AVehicle::StartBraking);
				Input->BindAction(BrakeInputAction, ETriggerEvent::Completed, this, &AVehicle::StopBraking);
				UE_LOG(DDLog, Log, TEXT("Bound Brake Input"))
				Input->BindAction(ShootInputAction, ETriggerEvent::Started, this, &AVehicle::StartTopMount);
				Input->BindAction(ShootInputAction, ETriggerEvent::Completed, this, &AVehicle::StopTopMount);
				UE_LOG(DDLog, Log, TEXT("Bound Shoot Input"))
				Input->BindAction(AimInputAction, ETriggerEvent::Triggered, this, &AVehicle::AimController);
				UE_LOG(DDLog, Log, TEXT("Bound Aiming Input"));
				Input->BindAction(BoostInputAction, ETriggerEvent::Started, this, &AVehicle::BoostPressed);
				Input->BindAction(BoostInputAction, ETriggerEvent::Completed, this, &AVehicle::BoostReleased);
				Input->BindAction(BoostInputAction, ETriggerEvent::Canceled, this, &AVehicle::BoostReleased);
				UE_LOG(DDLog, Log, TEXT("Bound Boosting Input"));
				Input->BindAction(ReloadInputAction, ETriggerEvent::Started, this, &AVehicle::ManualTryReloadTopMount);

				// Bind Input Switching delegate
				if (UInputDeviceSubsystem* InputDeviceSubsystem = GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>())
				{
					InputDeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &AVehicle::HandleInputDeviceChanged);
				}
			}
		}
	}	
}

void AVehicle::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Link AbilitySystem with Player State
	InitAbilitySystemComponent();
	// Grant abilities from Editor Defaults
	GiveDefaultAbilities();
	InitDefaultAttributes();
}

void AVehicle::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Link AbilitySystem with Player State
	InitAbilitySystemComponent();
	InitDefaultAttributes();
}

void AVehicle::BeginPlay()
{
	Super::BeginPlay();

	// Manually Check which Control Scheme is being used
	CheckInput(GetPlatformUserId());
	
	// Set miscellaneous Stats
	// Get GAS variables
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		CurrentTopSpeed = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetTopSpeedAttribute(), bFound);
		if (!bFound)
		{
			CurrentTopSpeed = 10.f;
			UE_LOG(DDLog, Warning, TEXT("TopSpeed Attribute not found in Vehicle ABSC"))
		}
	}

	// Setup Physics aspects of components

	VehiclePhysObj->SetSimulatePhysics(true);
	// Block all Collision by default
	VehiclePhysObj->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	VehiclePhysObj->SetCollisionResponseToAllChannels(ECR_Block);
	// Except Vacuumable and CursorBlocked, which should be overlap (GameTrace1 is Vacuumable, 2 is HitBox, 3 is CursorBlocked)
	VehiclePhysObj->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
	VehiclePhysObj->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore);
	VehiclePhysObj->SetMassOverrideInKg(NAME_None, 7000, true);
	// Set Vehicle Damping
	VehiclePhysObj->SetLinearDamping(1.f);
	VehiclePhysObj->SetPhysicsMaxAngularVelocityInDegrees(40);

	// Do not move the vehicle when damaged by a bug
	VehiclePhysObj->bApplyImpulseOnDamage = false;
	
	// Other Physics Aspects (With Validity Checks)
	if (Chassis)
	{	
		Chassis->SetCollisionResponseToAllChannels(ECR_Ignore);
		Chassis->SetCollisionProfileName(FName("NoCollision"));
		Chassis->SetMassOverrideInKg(NAME_None, 0.001, true);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Chassis Mesh not Valid on BeginPlay"));
	}
	
	// Attach Top Mount CORRECTLY
	FAttachmentTransformRules MountRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	if (IsValid(TopMount))
	{
		TopMount->AttachToComponent(Chassis, MountRules, FName("Turret_Socket"));
	}
	
	if (FrontMount)
	{
		FrontMount->AttachToComponent(Chassis, MountRules, FName("Bumper_Socket"));
		FrontMount->SetCollisionResponseToAllChannels(ECR_Ignore);
		FrontMount->SetCollisionProfileName(FName("NoCollision"));
		FrontMount->SetMassOverrideInKg(NAME_None, 0.001, true);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Bumper Mesh not Valid on BeginPlay"));
	}
	
	if (RearMount)
	{
		RearMount->AttachToComponent(Chassis, MountRules, FName("Tank_Socket"));
		RearMount->SetCollisionResponseToAllChannels(ECR_Ignore);
		RearMount->SetCollisionProfileName(FName("NoCollision"));
		RearMount->SetMassOverrideInKg(NAME_None, 0.001, true);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Bumper Mesh not Valid on BeginPlay"));
	}

	if (EngineMount)
	{
		EngineMount->AttachToComponent(Chassis, MountRules, FName("Engine_Socket"));
		EngineMount->SetCollisionResponseToAllChannels(ECR_Ignore);
		EngineMount->SetCollisionProfileName(FName("NoCollision"));
		EngineMount->SetMassOverrideInKg(NAME_None, 0.001, true);
	}
	else
	{
		UE_LOG(DDLog, Warning, TEXT("Engine Mesh not Valid on BeginPlay"));
	}

	if (WheelFL)
	{
		WheelFL->SetCollisionResponseToAllChannels(ECR_Ignore);
		WheelFL->SetCollisionProfileName(FName("NoCollision"));
		WheelFL->SetMassOverrideInKg(NAME_None, 0.001, true);
	}

	if (WheelFR)
	{
		WheelFR->SetCollisionResponseToAllChannels(ECR_Ignore);
		WheelFR->SetCollisionProfileName(FName("NoCollision"));
		WheelFR->SetMassOverrideInKg(NAME_None, 0.001, true);
	}

	if (WheelBL)
	{
		WheelBL->SetCollisionResponseToAllChannels(ECR_Ignore);
		WheelBL->SetCollisionProfileName(FName("NoCollision"));
		WheelBL->SetMassOverrideInKg(NAME_None, 0.001, true);
	}

	if (WheelBR)
	{
		WheelBR->SetCollisionResponseToAllChannels(ECR_Ignore);
		WheelBR->SetCollisionProfileName(FName("NoCollision"));
		WheelBR->SetMassOverrideInKg(NAME_None, 0.001, true);
	}

	// Error handling for Empty Tags
	// If any one Tag is empty, the car CANNOT function correctly. Throw an Error and Close the game
	if (AcceleratingTag == FGameplayTag::EmptyTag ||
		BrakingTag == FGameplayTag::EmptyTag ||
		GroundedTag == FGameplayTag::EmptyTag ||
		ReversingTag == FGameplayTag::EmptyTag ||
		SteeringTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(DDLog, Fatal, TEXT("Vehicle is missing One or More GameplayTag Definitions. Ensure all Tags in the Blueprint Class Are Allocated! Terminating..."));
	}

	// Bind Delegates for attribute changes
	if (AbilitySystemComponent)
	{
		// Acceleration
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AbilitySystemComponent->GetSet<UVehicleAttributeSet>()->GetAccelerationModifierAttribute()).AddUObject(this, &AVehicle::OnAccelerationAttributeChanged);
		// Handling
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AbilitySystemComponent->GetSet<UVehicleAttributeSet>()->GetHandlingAttribute()).AddUObject(this, &AVehicle::OnHandlingAttributeChanged);
		// TopSpeed
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AbilitySystemComponent->GetSet<UVehicleAttributeSet>()->GetTopSpeedAttribute()).AddUObject(this, &AVehicle::OnTopSpeedAttributeChanged);
	}
}

void AVehicle::GiveDefaultAbilities()
{
	if (AbilitySystemComponent)
	{
		// This should only trigger with authority, so save the function calls by returning here
		if (!HasAuthority())
		{
			return;
		}

		// For each ability in the DefaultAbilities Array (EditorExposed)
		for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
		{
			int32 NewAbilityID = -1;
			// If our new ability has an ID (custom gameplay ability type)
			if (UDDGameplayAbility* DDGameplayAbility = Cast<UDDGameplayAbility>(AbilityClass.GetDefaultObject()))
			{
				// Save for Assignment
				NewAbilityID = static_cast<int32>(DDGameplayAbility->AbilityID);
			}

			// Create an AbilitySpec with level 1, an InputID of -1 unless otherwise set, and this as it's owner object
			const FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, NewAbilityID, this);
			// Grant the Ability to the Vehicle (without Activating it)
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}
}

void AVehicle::InitDefaultAttributes() const
{
	// Safety check
	if (!AbilitySystemComponent || !DefaultAttributeEffect)
	{
		return;
	}

	// Create Gameplay Effect Context
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// Get Specification Handle
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, EffectContext);

	// Add the Specifications to the Player
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UAbilitySystemComponent* AVehicle::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UVehicleAttributeSet* AVehicle::GetAttributeSet() const
{
	return AttributeSet;
}

void AVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Slow to Stop when not accelerating
	if (!AbilitySystemComponent->HasMatchingGameplayTag(AcceleratingTag) && AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
	{
		float BrakingMod = (AbilitySystemComponent->HasMatchingGameplayTag(BrakingTag)) ? 5.f : 1.f;
		// Return Vehicle Speed to 0 over .2 Seconds
		Acceleration = FMath::FInterpTo(Acceleration, 0.f, DeltaSeconds * 2.f * BrakingMod, 1.f);
		AccelerationInput = FMath::FInterpTo(AccelerationInput, 0.f, DeltaSeconds * 2.f * BrakingMod, 1.f);

		// If Not Accelerating, try to bring the car to a stop
		// Apply force in the opposite Direction of the current forward force (force / tick) (NOT GRAVITY)
		FVector CurrentVehicleVelocity = VehiclePhysObj->GetPhysicsLinearVelocity();
		CurrentVehicleVelocity.Z = 0.f;
		VehiclePhysObj->AddForce(-5.f * CurrentVehicleVelocity, NAME_None, true);
	}
	// If not trying to steer, or is currently airborne
	if ((!AbilitySystemComponent->HasMatchingGameplayTag(SteeringTag) || !AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag)))
	{
		// Return Steering Rate to 0 over .1 seconds
		SteerInput = FMath::FInterpTo(SteerInput, 0.f, DeltaSeconds * 4.f, 1.f);
		SteerSpeed = FMath::FInterpTo(SteerSpeed, 0.f, DeltaSeconds * 4.f, 1.f);
	}

	// Update Camera Position to be where the player is moving to (based on vector)
	if (IsValid(SweepCameraBoom) && IsValid(CameraBoom) && IsValid(Camera))
	{
		// Camera Movement and Zoom
		MoveCamera(DeltaSeconds);
	}

	// Turret Rotation changes with input style
	if (!bUsingController)
	{
		// Mouse and Keyboard
		AimMouse();
	}

	// Limit Vehicle Velocity
	DoSpeedLimit(DeltaSeconds);
	
	// Prevent the vehicle from rolling or flipping
	LockVehicleRotation();
	// Update Wheel Rotation
	UpdateWheelRotation(DeltaSeconds, Acceleration * AccelerationInput);
	// Suspension Calculations
	DoSuspension(DeltaSeconds);

	// Do triggering of Start/Stop Driving
	FVector LateralVelocity = FVector(VehiclePhysObj->GetComponentVelocity().X, VehiclePhysObj->GetComponentVelocity().Y, 0.f);
	if (LateralSpeedLastTick <= 10.f && LateralVelocity.Length() > 10.f)
	{
		// Start Driving
		OnVehicleStartDrive();
	}
	if (LateralSpeedLastTick > 10.f && LateralVelocity.Length() <= 10.f)
	{
		OnVehicleStopDrive();
	}
	LateralSpeedLastTick = LateralVelocity.Length();
	
	// Update Dirty bool values, Prevents Accelerating and Steering multiple times per tick
	bAccelDirty = false;
	bSteerDirty = false;
}

void AVehicle::UpdateWheelRotation(float DeltaTime, float CurrentMoveSpeed) const
{
	// Get GAS Handling
	float Handling = 0.f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		Handling = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetHandlingAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("Handling Attribute not found in Vehicle ABSC"))
		}
	}

	if (!IsValid(WheelFL) || !IsValid(WheelBL))
	{
		UE_LOG(DDLog, Error, TEXT("Cannot Rotate wheels, Mesh is incorrect"));
		return;
	}

	// Rotate Mesh Forwards Backwards
	// Rotate Knuckle Left/Right
	// Front wheel circumference rotation
	float FrontWheelCircumference = 2 * 3.1416 * WheelFL->GetStaticMesh()->GetBounds().SphereRadius;
	// Get Rotation Amount in Degrees
	float FrontWheelForwardRotation = ((CurrentMoveSpeed) / FrontWheelCircumference) * 360;
	// Front wheel directional rotation
	float FrontWheelNewSteerRotation = SteerInput * FMath::Min(Handling * 0.3f, 40.f);

	// Rear wheel circumference rotation
	float BackWheelCircumference = 2 * 3.1416 * WheelBL->GetStaticMesh()->GetBounds().SphereRadius;
	// Get Rotation Amount in Degrees
	float BackWheelForwardRotation = ((CurrentMoveSpeed) / BackWheelCircumference) * 360;

	FQuat FrontDeltaRotation = FQuat(KnuckleFL->GetRightVector()*-1, FMath::DegreesToRadians(FrontWheelForwardRotation));
	FQuat BackDeltaRotation = FQuat(KnuckleBL->GetRightVector()*-1, FMath::DegreesToRadians(BackWheelForwardRotation));

	WheelFL->AddWorldRotation(FrontDeltaRotation);
	WheelFR->AddWorldRotation(FrontDeltaRotation);
	WheelBL->AddWorldRotation(BackDeltaRotation);
	WheelBR->AddWorldRotation(BackDeltaRotation);

	// Steer Front Wheels
	KnuckleFL->SetRelativeRotation(FRotator(0.f, FrontWheelNewSteerRotation, 0.f));
	KnuckleFR->SetRelativeRotation(FRotator(0.f, FrontWheelNewSteerRotation, 0.f));
}

void AVehicle::DoSuspension(float DeltaTime)
{
	// If suspension Length has not been set, set it here.
	if (SuspensionLength <= 0)
	{
		if (!IsValid(WheelFL))
		{
			UE_LOG(DDLog, Error, TEXT("Suspension Calculation failed, Wheel setup incorrectly"))
			return;
		}
		// If Front Left wheel has no mesh, ignore suspension
		if (UStaticMesh* Mesh = WheelFL->GetStaticMesh())
		{
			// Suspension Length = Front Wheel Radius
			SuspensionLength = Mesh->GetBounds().SphereRadius * 2.f;
		}
		else
		{
			// If no wheel mesh exists, skip this function entirely, it requires teh wheel meshes to exist.
			UE_LOG(DDLog, Error, TEXT("Front Left Wheel Mesh has not been set, The Vehicle cannot see the ground!"))
			return;
		}
	}

	// TODO: Re-implement Suspension to prevent the car turning with no input

	// Grounded Wheel Count
	int GroundedWheels = 0;

	UWorld* World = GetWorld();

	// Reusable Hit Result
	FHitResult SuspensionHit;

	// Do Wheel Locations for suspension points (Re-use later for HitLocations)
	FVector PosFL = WheelFL->GetComponentLocation();
	FVector PosFR = WheelFR->GetComponentLocation();
	FVector PosBL = WheelBL->GetComponentLocation();
	FVector PosBR = WheelBR->GetComponentLocation();

	// Ignore Self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Draw a Line trace from each wheel down the Suspension length, to check if the vehicle is on (near) the ground

	// Front Left Wheel
	GroundedWheels += World->LineTraceSingleByChannel(SuspensionHit, PosFL, PosFL - FVector(0.f, 0.f, SuspensionLength), ECC_WorldStatic, QueryParams);
#if WITH_EDITOR
	DrawDebugLine(GetWorld(), PosFL, PosFL - FVector(0.f, 0.f, SuspensionLength), FColor::Red);
#endif
	// Also get the Normal vector of the ground that was hit by the suspension (used in the veering prevention)
	PosFL = SuspensionHit.ImpactNormal;
	
	// Front Right Wheel
	GroundedWheels += World->LineTraceSingleByChannel(SuspensionHit, PosFR, PosFR - FVector(0.f, 0.f, SuspensionLength), ECC_WorldStatic, QueryParams);
#if WITH_EDITOR
	DrawDebugLine(GetWorld(), PosFR, PosFR - FVector(0.f, 0.f, SuspensionLength), FColor::Red);
#endif
	// Also get the Normal vector of the ground that was hit by the suspension (used in the veering prevention)
	PosFR = SuspensionHit.ImpactNormal;

	// Back Left Wheel
	GroundedWheels += World->LineTraceSingleByChannel(SuspensionHit, PosBL, PosBL - FVector(0.f, 0.f, SuspensionLength), ECC_WorldStatic, QueryParams);
#if WITH_EDITOR
	DrawDebugLine(GetWorld(), PosBL, PosBL - FVector(0.f, 0.f, SuspensionLength), FColor::Red);
#endif
	// Also get the Normal vector of the ground that was hit by the suspension (used in the veering prevention)
	PosBL = SuspensionHit.ImpactNormal;
	
	// Back Right Wheel
	GroundedWheels += World->LineTraceSingleByChannel(SuspensionHit, PosBR, PosBR - FVector(0.f, 0.f, SuspensionLength), ECC_WorldStatic, QueryParams);
#if WITH_EDITOR
	DrawDebugLine(GetWorld(), PosBR, PosBR - FVector(0.f, 0.f, SuspensionLength), FColor::Red);
#endif
	// Also get the Normal vector of the ground that was hit by the suspension (used in the veering prevention)
	PosBR = SuspensionHit.ImpactNormal;

	// Determine whether the vehicle is Grounded
	// Grant or remove tag based on whether the vehicle should be considered grounded or not
	if (GroundedWheels > 1)
	{
		// The Vehicle is grounded
		// If we Don't have the tag, add it
		if (!AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
		{
			AbilitySystemComponent->AddLooseGameplayTag(GroundedTag);
		}
	}
	else
	{
		// The Vehicle is airborne
		// If we have the tag, remove it
		if (AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(GroundedTag);
		}
	}
	
	// Max number of wheels is 4, if all 4 wheels are on the ground
	if (GroundedWheels == 4)
	{
		// Set Angular Momentum to zero (Prevents lateral Veering)
		VehiclePhysObj->SetAllPhysicsAngularVelocityInDegrees(FVector(0, VehiclePhysObj->GetPhysicsAngularVelocityInDegrees().Y, 0));
	}
}

void AVehicle::LockVehicleRotation() const
{
	// Get Up Vector
	FVector VehicleUpVector = VehiclePhysObj->GetUpVector();

	// Dot Product the two vectors together
	float UpDot = FVector::DotProduct(FVector(0, 0, 1), VehicleUpVector);
	float UpDeg = FMath::RadiansToDegrees(FMath::Acos(UpDot));

	// If the vehicle is (non-specifically) rotated beyond a certain threshold it may need to be rotated back into place
	if (UpDeg > 30.f)
	{
		// The Rotation is over what should be normal, figure out specifically which direction overdone and dial it back a little.
		FVector VehicleWorldRightVector = FVector(VehiclePhysObj->GetRightVector().X, VehiclePhysObj->GetRightVector().Y, 0.f).GetSafeNormal();
		FVector VehicleWorldForwardVector = FVector(VehiclePhysObj->GetForwardVector().X, VehiclePhysObj->GetForwardVector().Y, 0.f).GetSafeNormal();

		FVector VehicleRightVector = VehiclePhysObj->GetRightVector();
		FVector VehicleForwardVector = VehiclePhysObj->GetForwardVector();

		float RightDot = FVector::DotProduct(VehicleWorldRightVector, VehicleRightVector);
		float RightDeg = FMath::RadiansToDegrees(FMath::Acos(RightDot));

		float ForwardDot = FVector::DotProduct(VehicleWorldForwardVector, VehicleForwardVector);
		float ForwardDeg = FMath::RadiansToDegrees(FMath::Acos(ForwardDot));

		FRotator RotationToApply = FRotator(0);

		// If Player is tilted too far either to the left or the right (Roll)
		if (RightDeg > 30.f)
		{
			// Determine which way the vehicle is rolling (around the X axis)
			if (VehiclePhysObj->GetComponentRotation().Roll < 0)
			{
				// Rolled towards the right, counter by rolling left
				RotationToApply.Roll += (RightDeg - 30.f);
			}
			else
			{
				// Rolled towards the left, counter by rolling right
				RotationToApply.Roll -= (RightDeg - 30.f);
			}
		}

		// Player is tilted too far either forward or backward (Pitch)
		if (ForwardDeg > 40.f)
		{
			// Determine which way the vehicle is Pitching (around the Y axis)
			if (VehiclePhysObj->GetComponentRotation().Pitch < 0)
			{
				// Pitching Down, counter by pitching up
				RotationToApply.Pitch += (ForwardDeg - 40.f);
			}
			else
			{
				// Pitching up, counter by Pitching Down
				RotationToApply.Pitch -= (ForwardDeg - 40.f);
			}
		}

		// Rotate the vehicle as necessary (sweep to do blocking, to help prevent clipping)
		VehiclePhysObj->AddLocalRotation(RotationToApply, true);
	}
}

void AVehicle::DoSpeedLimit(float DeltaTime)
{
	// If the player is not grounded, don't bother with speed limitations (test)
	if (!AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
	{
		return;
	}
	
	// Get the Current Velocity of the Vehicle (except for the Vertical velocity, ignore up/down)
	FVector CurrentVelocity = VehiclePhysObj->GetComponentVelocity();
	CurrentVelocity.Z = 0.f;
	float CurrentSpeed = CurrentVelocity.Length();
	if (CurrentSpeed > CurrentTopSpeed * 100.f)// && CurrentVelocity.Length() > 10.f)
	{
		// Get the Amount of difference.
		float ExcessSpeed = CurrentSpeed - (CurrentTopSpeed * 100.f);

		// Reduce Speed by 40% of Excess Speed per second 
		float SpeedToReduce = ExcessSpeed * 0.4f * DeltaTime;
		// if moving at double the allotted speed, reduce speed this tick by 80% of difference per second instead
		if (ExcessSpeed > CurrentTopSpeed * 200.f)
		{
			SpeedToReduce *= 2.f;
		}
		
		// Clamp the Velocity of the Vehicle
		FVector UnitVelocity = CurrentVelocity.GetSafeNormal();
		UnitVelocity *= (CurrentSpeed - (SpeedToReduce));
		VehiclePhysObj->SetPhysicsLinearVelocity(FVector(UnitVelocity.X, UnitVelocity.Y, VehiclePhysObj->GetComponentVelocity().Z), false);
	}
}

void AVehicle::MoveCamera(float DeltaTime)
{
	/* New system concept
	 *
	 * World yaw Rotation is towards aim rotation, taken as the Yaw of the TopMount (-180)
	 *
	 * Arm length is set using the cursor hit location, limited by a ratio of the distance of the other CameraBoomArm (3:1)
	 * 
	 * SweepCameraBoom yaw rotation and TargetLength lerp towards these values
	 */

	// Setup variables for use
	float NewTargetYaw = 0.f;
	float NewTargetLength = 0.f;

	// Get Target Rotation
	if (IsValid(TopMount))
	{
		NewTargetYaw = TopMount->GetTurretMesh()->GetComponentRotation().Yaw - 180.f;
	}

	// Get TargetLength (PreClamp)
	FHitResult HitResult;
	GetController<APlayerController>()->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel3, false, HitResult);

	// Update Cursor Hit Location
	CursorHitLoc = HitResult.Location;

	// Clamp Target length
	NewTargetLength = FMath::Clamp(CameraSweepAggression, 0, 1) * FVector::Distance(FVector(GetActorLocation().X, GetActorLocation().Y, 0), FVector(HitResult.Location.X, HitResult.Location.Y, 0));

	// Clamp Target Length (1/3 of Outer Camera length maximum)
	NewTargetLength = FMath::Min(NewTargetLength, CameraBoom->TargetArmLength * 0.33f);

	// Lerp SweepCameraBoom rotation
	FRotator TargetRot = FRotator(SweepCameraBoom->GetComponentRotation().Pitch, NewTargetYaw, SweepCameraBoom->GetComponentRotation().Roll);
	FRotator InterpRot = FMath::RInterpTo(SweepCameraBoom->GetComponentRotation(), TargetRot, DeltaTime, FMath::Clamp(CameraSweepRate, 0, 10) * 2.f);
	SweepCameraBoom->SetWorldRotation(InterpRot);

	// Lerp SweepCameraBoom length
	SweepCameraBoom->TargetArmLength = FMath::FInterpTo(SweepCameraBoom->TargetArmLength, NewTargetLength, DeltaTime, FMath::Clamp(CameraSweepRate, 0, 10));
}

void AVehicle::Accelerate(const struct FInputActionValue& Value)
{
	if (bAccelDirty)
	{
		// Do not accelerate twice in one tick
		return;
	}
	
	// If Airborne, do not accept Acceleration Inputs
	if (!AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
	{
		// If we have accelerating tag, remove it
		if (AbilitySystemComponent->HasMatchingGameplayTag(AcceleratingTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(AcceleratingTag);
		}
		return;
	}

	// If we don't have accelerating tag, add it
	if (!AbilitySystemComponent->HasMatchingGameplayTag(AcceleratingTag))
	{
		AbilitySystemComponent->AddLooseGameplayTag(AcceleratingTag);
	}

	float AccelDir = Value.Get<float>();

	// If Reversing
	if (AccelerationInput < 0)
	{
		// If we don't have reversing tag, add it
		if (!AbilitySystemComponent->HasMatchingGameplayTag(ReversingTag))
		{
			AbilitySystemComponent->AddLooseGameplayTag(ReversingTag);
		}
	}
	else
	{
		// If we have reversing tag, remove it
		if (AbilitySystemComponent->HasMatchingGameplayTag(ReversingTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(ReversingTag);
		}
	}
	
	FVector ForwardVector = VehiclePhysObj->GetForwardVector();
	ForwardVector.Z = 0.f;
	ForwardVector.Normalize();

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	// The rate at which the acceleration can change, used when changing from forward to reverse or vice versa
	// Get from GAS

	float ChangeMulti = 0.5f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		ChangeMulti = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetAccelerationModifierAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("AccelerationModifier Attribute not found in Vehicle ABSC"))
		}
	}

	// If we are suddenly changing direction multiply the change in direction rate
	if ((AccelerationInput < 0 && AccelDir > 0 ) || (AccelerationInput > 0 && AccelDir < 0))
	{
		ChangeMulti *= 4;
		// Car is Getting Slower before getting faster, no longer accelerating
		// If we have accelerating tag, remove it
		if (AbilitySystemComponent->HasMatchingGameplayTag(AcceleratingTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(AcceleratingTag);
		}
	}
	// Calculate Acceleration Interpolation
	AccelerationInput = FMath::FInterpTo(AccelerationInput, AccelDir, DeltaTime * ChangeMulti, 2.f);
	Acceleration = FMath::FInterpTo(Acceleration, CurrentTopSpeed, DeltaTime, 2.f);

	FVector ForceToApply = ForwardVector * Acceleration * AccelerationInput * 100 * VehiclePhysObj->GetMass();

	// Add force to each wheel if below a certain speed
	FVector MovementVelocity = VehiclePhysObj->GetComponentVelocity();
	float CurrentSpeed = FVector(MovementVelocity.X, MovementVelocity.Y, 0.f).Length();
	if (CurrentSpeed < CurrentTopSpeed * 100)
	{
		// Apply force to vehicle at location of each of the knuckles
		VehiclePhysObj->AddForceAtLocation(ForceToApply, KnuckleBL->GetComponentLocation());
		VehiclePhysObj->AddForceAtLocation(ForceToApply, KnuckleBR->GetComponentLocation());

		// Additionally rotate the front wheel ones, based on steering rotation
		FVector RotatedForceToApply = ForceToApply.RotateAngleAxis(KnuckleFL->GetRelativeRotation().Yaw, VehiclePhysObj->GetUpVector());
		VehiclePhysObj->AddForceAtLocation(RotatedForceToApply, KnuckleFL->GetComponentLocation());
		VehiclePhysObj->AddForceAtLocation(RotatedForceToApply, KnuckleFR->GetComponentLocation());
	}
	bAccelDirty = true;
}

void AVehicle::StartAccelerating()
{
	// Invalidate SteeringEaseOutSteeringTimer
	if (AccelEaseOutHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(AccelEaseOutHandle);
		AccelEaseOutHandle.Invalidate();
	}
}

void AVehicle::StopAccelerating()
{
	// If we have accelerating tag, remove it
	if (AbilitySystemComponent->HasMatchingGameplayTag(AcceleratingTag))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(AcceleratingTag);
	}
	// If we have reversing tag, remove it
	if (AbilitySystemComponent->HasMatchingGameplayTag(ReversingTag))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(ReversingTag);
	}

	// Ease out Acceleration, stop slowly
	int CallCount = 0;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("EaseOutAccel"), CallCount);
	AccelEaseOutHandle = GetWorldTimerManager().SetTimerForNextTick(TimerDel);
}

void AVehicle::Steer(const FInputActionValue& Value)
{
	if (bSteerDirty)
	{
		// Steering was already called this tick, don't double steer
		return;
	}
	
	// If we don't have steering tag, add it
	if (!AbilitySystemComponent->HasMatchingGameplayTag(SteeringTag))
	{
		AbilitySystemComponent->AddLooseGameplayTag(SteeringTag);
	}

	// Get GAS Handling
	float Handling = 0.f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		Handling = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetHandlingAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("Handling Attribute not found in Vehicle ABSC"))
		}
	}

	// Range from -1 to 1, -1 is turn left, 1 is turn right
	float SteerVal = Value.Get<float>();

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	// The rate at which the steering can change, increased when changing from left to right or vice versa
	float ChangeMulti = SteeringAcceleration;
	// If we are suddenly changing steering direction multiply the change in direction rate
	if ((SteerInput < 0.5f && SteerVal > 0) || (SteerInput> -0.5f && SteerVal < 0))
	{
		ChangeMulti *= 4;
	}
	// Calculate Steering Speed
	SteerInput = FMath::FInterpTo(SteerInput, SteerVal, DeltaTime * ChangeMulti, 1.f);
	SteerSpeed = FMath::FInterpTo(SteerSpeed, Handling, DeltaTime * ChangeMulti, 1.f);

	// If the car is off the ground, and the brakes are off
	if (!AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
	{
		// Don't turn the vehicle
		return;
	}

	// Get GAS Top Speed
	float TopSpeed = 0.f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		TopSpeed = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetTopSpeedAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("TopSpeed Attribute not found in Vehicle ABSC"))
		}
	}

	// Add Rotation to the Vehicle
	FRotator RotationToAdd = FRotator(0.f);
	/*
	 * SteerInput = Direction (Interpolated value to InputActionValue)
	 * bIsReversing (Flip the direction if the vehicle is reversing)
	 * SteerSpeed = Amount of Steering (Interpolated to Handling Value)
	 * DeltaTime = Time this Tick
	 * if the brakes are applied
	 *	Min(Accel/TopSpeed, 1) (The Steering speed is based on the acceleration of the vehicle)
	 * otherwise
	 *	Acceleration / TopSpeed (The Speed of the Vehicle compared to Top Speed as a 0-1 ratio)
	 */
	RotationToAdd.Yaw = SteerInput * (AbilitySystemComponent->HasMatchingGameplayTag(ReversingTag) ? -1 : 1) * SteerSpeed * DeltaTime * (!AbilitySystemComponent->HasMatchingGameplayTag(BrakingTag) ? (Acceleration * 1 / TopSpeed) : (FMath::Min(Acceleration / CurrentTopSpeed, 1.f)) * 1.5f);
	VehiclePhysObj->AddWorldRotation(RotationToAdd);

	bSteerDirty = true;
}

void AVehicle::StartSteering()
{
	// Invalidate SteeringEaseOutSteeringTimer
	if (SteeringEaseOutHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SteeringEaseOutHandle);
		SteeringEaseOutHandle.Invalidate();
	}
}

void AVehicle::StopSteering()
{
	// If we have steering tag, remove it.
	if (AbilitySystemComponent->HasMatchingGameplayTag(SteeringTag))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(SteeringTag);
	}

	// Start EaseOutSteeringTimer
	int CallCount = 0;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("EaseOutSteering"), CallCount);
	SteeringEaseOutHandle = GetWorldTimerManager().SetTimerForNextTick(TimerDel);
}

void AVehicle::HandleInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	// Call the Seperated Version of this Function
	CheckInput(UserId);
}

void AVehicle::CheckInput(const FPlatformUserId UserId)
{
	// If UserID is invalid, return early and Log an Output 
	if (!UserId.IsValid())
	{
		UE_LOG(DDLog, Error, TEXT("Vehicle->CheckInput: Invalid UserId"));
		return;
	}
	
	if (UInputDeviceSubsystem* InputDeviceSubsystem = GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>())
	{
		EHardwareDevicePrimaryType NewDeviceType = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(UserId).PrimaryDeviceType;
		bUsingController = (NewDeviceType == EHardwareDevicePrimaryType::Gamepad);
		OnInputChanged(NewDeviceType);
		// Debug Output
		FString DeviceInputType;
		switch (NewDeviceType)
		{
		case EHardwareDevicePrimaryType::Gamepad:
			DeviceInputType = "Gamepad";
			break;
		case EHardwareDevicePrimaryType::Camera:
			DeviceInputType = "Camera";
			break;
		case EHardwareDevicePrimaryType::FlightStick:
			DeviceInputType = "FlightStick";
			break;
		case EHardwareDevicePrimaryType::Instrument:
			DeviceInputType = "Instrument";
			break;
		case EHardwareDevicePrimaryType::KeyboardAndMouse:
			DeviceInputType = "Keyboard and Mouse";
			break;
		case EHardwareDevicePrimaryType::RacingWheel:
			DeviceInputType = "Racing Wheel";
			break;
		case EHardwareDevicePrimaryType::MotionTracking:
			DeviceInputType = "Motion Tracking";
			break;
		case EHardwareDevicePrimaryType::Touch:
			DeviceInputType = "Touch";
			break;
		case EHardwareDevicePrimaryType::Unspecified:
			DeviceInputType = "Unspecified";
			break;
		default:
			DeviceInputType = "Custom";
		}
		UE_LOG(DDLog, Log, TEXT("Device Type is: %s"), *DeviceInputType)
	}
}

void AVehicle::EaseOutSteering(int CallCount)
{
	// Continue Rotating the car, while it's rotation decreases
	FRotator RotationToAdd = FRotator(0);

	// Get GAS Top Speed
	float TopSpeed = 0.f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		TopSpeed = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetTopSpeedAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("TopSpeed Attribute not found in Vehicle ABSC"))
		}
	}

	RotationToAdd.Yaw = SteerInput * (AbilitySystemComponent->HasMatchingGameplayTag(ReversingTag) ? -1 : 1) * SteerSpeed * GetWorld()->GetDeltaSeconds() * (!AbilitySystemComponent->HasMatchingGameplayTag(BrakingTag) ? (Acceleration / TopSpeed) : (FMath::Min(Acceleration / CurrentTopSpeed, 1.f)));
	VehiclePhysObj->AddWorldRotation(RotationToAdd);

	// Call this function for 1.5 seconds
	if (CallCount < (1.5f/ GetWorld()->GetDeltaSeconds()))
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("EaseOutSteering"), ++CallCount);
		SteeringEaseOutHandle = GetWorldTimerManager().SetTimerForNextTick(TimerDel);
	}
}

void AVehicle::EaseOutAccel(int CallCount)
{
	// If at any point the vehicle leaves the ground, stop easing out the acceleration
	if (!AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag))
	{
		return;
	}
	// Continue applying force to the car
	FVector ForwardVector = VehiclePhysObj->GetForwardVector();
	ForwardVector.Z = 0.f;
	ForwardVector.Normalize();

	FVector ForceToApply = ForwardVector * Acceleration * AccelerationInput * 100 * VehiclePhysObj->GetMass();

	// Add force to each wheel if below a certain speed
	FVector MovementVelocity = VehiclePhysObj->GetComponentVelocity();
	float CurrentSpeed = FVector(MovementVelocity.X, MovementVelocity.Y, 0.f).Length();
	if (CurrentSpeed < CurrentTopSpeed * 100)
	{
		// Apply force to vehicle for wheels
		VehiclePhysObj->AddForce(4 * ForceToApply);
	}

	// Call this function over 3 seconds
	if (CallCount < (3.f / GetWorld()->GetDeltaSeconds()))
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("EaseOutAccel"), ++CallCount);
		AccelEaseOutHandle = GetWorldTimerManager().SetTimerForNextTick(TimerDel);
	}
}

void AVehicle::StartBraking()
{
	if (!AbilitySystemComponent->HasMatchingGameplayTag(BrakingTag))
	{
		OnVehicleBrakesApplied();
		AbilitySystemComponent->AddLooseGameplayTag(BrakingTag);
	}
	
	float TopSpeed = 0.f;
	if (AbilitySystemComponent)
	{
		TopSpeed = AbilitySystemComponent->GetNumericAttributeBase(UVehicleAttributeSet::GetTopSpeedAttribute());
		if (TopSpeed == 0.f)
		{
			UE_LOG(DDLog, Warning, TEXT("TopSpeed Attribute not found in Vehicle ABSC"))
			// Set TopSpeed to default value here, to fix conversion
			TopSpeed = 35.f;
		}
	}
	// Cut top speed by a third. (35 down to 12~, 2x as fast as previous iteration)
	CurrentTopSpeed = TopSpeed / 3.f;
}

void AVehicle::StopBraking()
{
	// Get GAS TopSpeed
	float TopSpeed = 0.f;
	if (AbilitySystemComponent)
	{
		bool bFound = false;
		TopSpeed = AbilitySystemComponent->GetGameplayAttributeValue(UVehicleAttributeSet::GetTopSpeedAttribute(), bFound);
		if (!bFound)
		{
			UE_LOG(DDLog, Warning, TEXT("TopSpeed Attribute not found in Vehicle ABSC"))
		}
	}

	// Remove Braking Tag
	if (AbilitySystemComponent->HasMatchingGameplayTag(BrakingTag))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(BrakingTag);
		OnVehicleBrakesReleased();
	}
	CurrentTopSpeed = TopSpeed;
}

void AVehicle::BoostPressed()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EDDAbilityInputID::Boost));
	}
}

void AVehicle::BoostReleased()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EDDAbilityInputID::Boost));
	}
}

void AVehicle::StartTopMount()
{
	// If the Vehicle has a Top Mount Weapon, tell the weapon to start firing
	if (IsValid(TopMount))
	{
		TopMount->StartUse();
	}
}

void AVehicle::ManualTryReloadTopMount()
{
	TryReloadTopMount(R_MANUAL);
}

void AVehicle::TryReloadTopMount(EReloadType ReloadType) const
{
	// If the vehicle has a valid Top Mount (Turret)
	if (IsValid(TopMount))
	{
		// Attempt to Reload the TopMount, parsing reload type for extra functionality
		TopMount->TryReload(ReloadType);
	}
}

void AVehicle::AimController(const struct FInputActionValue& Value)
{
	// If the Vehicle has a valid Top Mount (Turret)
	if (IsValid(TopMount))
	{

		// Process Input Action
		FVector2D AimInput = Value.Get<FVector2D>();

		// Convert to 3d Vector to use in helper function
		FVector AimInput3D = FVector(AimInput.X, AimInput.Y, 0.f).GetSafeNormal();

		float AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AimInput3D.X, AimInput3D.Y));
		// Get Angle of rotation

		// Tell Turret to rotate
		TopMount->Aim(AimAngle);
	}
}

void AVehicle::AimMouse() const
{
	// If TopMount is valid
	if (IsValid(TopMount) && IsValid(Controller))
	{
		// Get Location of Cursor in world.
		APlayerController* PC = Cast<APlayerController>(GetController());
		FHitResult HitResult;
		// Specifically have this be blocked by the CursorBlocked channel
		PC->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel3, false, HitResult);

		// Get Hit Location of cursor
		FVector HitLoc = HitResult.Location;

		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(TopMount->GetActorLocation(), HitLoc);

		// Tell Turret to rotate
		TopMount->Aim(NewRotation.Yaw);
	}
}

void AVehicle::StopTopMount()
{
	// If the Vehicle has a Top Mount Weapon, tell the weapon to stop firing
	if (IsValid(TopMount))
	{
		TopMount->StopUse();
	}
}

void AVehicle::InitAbilitySystemComponent()
{
	// Get the PlayerState
	if (ADustDevilPlayerState* DustDevilPlayerState = GetPlayerState<ADustDevilPlayerState>())
	{
		// Get the AbilitySystemComponent from the Player State
		if (UDD_AbilitySystemComponent* PlayerStateAbilitySystemComponent = Cast<UDD_AbilitySystemComponent>(DustDevilPlayerState->GetAbilitySystemComponent()))
		{
			// Copy to this ABS
			AbilitySystemComponent = PlayerStateAbilitySystemComponent;
			// Link this Pawn to the Player State
			AbilitySystemComponent->InitAbilityActorInfo(DustDevilPlayerState, this);
			AttributeSet = DustDevilPlayerState->GetAttributeSet();
		}
		else
		{
			UE_LOG(DDLog, Error, TEXT("Player State does not contain an AbilitySystemComponent"))
		}
	}
	else
	{
		UE_LOG(DDLog, Error, TEXT("PlayerState is not of type: ADustDevilPlayerState, GAS Non functional"))
	}
}

void AVehicle::OnAccelerationAttributeChanged(const struct FOnAttributeChangeData& Data)
{
	// Do Internal Attribute Change functionality
	// Which in this case is nothing, good to know
	// Then call the BP version for visual/sound effects if needed
	OnAccelerationChanged(Data.OldValue, Data.NewValue);
}

void AVehicle::OnHandlingAttributeChanged(const struct FOnAttributeChangeData& Data)
{
	// Do Internal Attribute Change functionality
	// Which in this case is nothing, also good to know
	// Then call the BP version for visual/sound effects if needed
	OnHandlingChanged(Data.OldValue, Data.NewValue);
}

void AVehicle::OnTopSpeedAttributeChanged(const struct FOnAttributeChangeData& Data)
{
	// Do Internal Attribute Change functionality
	CurrentTopSpeed = Data.NewValue;
	// Then call the BP version for visual/sound effects if needed
	OnTopSpeedChanged(Data.OldValue, Data.NewValue);
}

void AVehicle::SetTopMount(class ATopMountBase* NewMount)
{
	// If the Current Top Mount is valid, disable it first
	if (IsValid(TopMount))
	{
		TopMount->StopUse();
		TopMount->Destroy();
	}

	// Then replace the Mount locally
	TopMount = NewMount;

	// Turn off Collision and Physics
	TopMount->SetActorEnableCollision(false);
	TopMount->DisableComponentsSimulatePhysics();
	// Then weld the new TopMount to the Turret Socket
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	AttachRules.bWeldSimulatedBodies = true;
	TopMount->AttachToComponent(Chassis, AttachRules, FName(TEXT("Turret_Socket")));
}

ATopMountBase* AVehicle::GetTopMount() const
{
	// Return Top Mount if Valid
	return IsValid(TopMount) ? TopMount : nullptr;
}
	