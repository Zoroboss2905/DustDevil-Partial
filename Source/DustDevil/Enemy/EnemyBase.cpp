// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "AttackGroup.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "DustDevil/DustDevilCommonData.h"
#include "DustDevil/Core/DustDevilGameState.h"
#include "DustDevil/GAS/BaseAttributeSet.h"
#include "DustDevil/GAS/DDGameplayAbility.h"
#include "DustDevil/GAS/DD_AbilitySystemComponent.h"
#include "DustDevil/Interfaces/DDEnemyEventManager.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Collision for Capsule Component
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Traces
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	// Object
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	
	// Setup Hitbox, with proper Collision Channels
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	// Set Collision with all channels
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Set Collision with Exceptions (WorldDynamic, Vaccumable and HitBox)
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
	// Use Continuous Collision Detection (Helps with Low framerate hit detection)
	HitBox->SetUseCCD(true);
		
	// Attach Hurtbox to the mouth of the Enemy
	HurtBox = CreateDefaultSubobject<USphereComponent>(TEXT("HurtBox"));
	// TODO: Add a Joint Uniformly across all enemy meshes for Hurtbox location.
	HurtBox->SetupAttachment(GetMesh(), FName(""));
	HurtBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HurtBox->SetCollisionProfileName(FName("OverlapAllDynamic"));

	// Change Mesh Collision Settings
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Traces
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	// Object
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	
	// GAS Component
	AbilitySystemComponent = CreateDefaultSubobject<UDD_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// Replication for multiplayer (if we use it later)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBase::DrawDebugSymbols(float DeltaTime)
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), 64 * GetActorScale3D().X, 16, FColor::Red, false, DeltaTime*1.5f);
	DrawDebugLine(GetWorld(), GetActorLocation(), AttackGroup->GetActorLocation(), FColor::Red, false, DeltaTime*1.5f);
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int32 AEnemyBase::GetDamageableType_Implementation()
{
	return Types;
}

void AEnemyBase::InitialiseAbilities()
{
	// Authority Check
	if (!HasAuthority())
	{
		return;
	}
	// Safety Check
	if (!AbilitySystemComponent)
	{
		UE_LOG(DDLog, Error, TEXT("%s Failed to Initialise Gameplay Abilities: ABSC Invalid!"), *GetName());
		return;
	}
	
	// For each ability in DefaultAbilities
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
		// Grant the Ability to the Enemy (without Activating it)
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
	
}

void AEnemyBase::InitialiseEffects()
{
	// Safety check
	if (!AbilitySystemComponent)
	{
		UE_LOG(DDLog, Error, TEXT("%s Failed to Initialise Gameplay Effects: ABSC Invalid!"), *GetName())
		return;
	}

	// Create Gameplay Effect Context (Reusable)
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	// For each effect that should be applied
	for (TSubclassOf<UGameplayEffect> Effect : DefaultEffects)
	{
		// Get Specification Handle
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1.f, EffectContext);

		// Add the Effect by Spec Handle
		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AEnemyBase::ApplyInitialAttributes(TSubclassOf<UGameplayEffect> EffectClass, float NewHealth,
	float NewArmour, float NewDamage, float NewBioMatter, float NewNuclearChance, float NewChitinChance)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(DDLog, Error, TEXT("%s: ABSC Not Valid (ApplyInitialAttributes())"), *GetName());
		return;
	}

	// Create New Attribute Set here
	AbilitySystemComponent->InitStats(UBaseAttributeSet::StaticClass(), nullptr);
	
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(this, this);
	
	// Once Spec is valid, Populate with Set Caller by Tag
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, EffectContext);
	
	if (!SpecHandle.IsValid())
	{
		UE_LOG(DDLog, Error, TEXT("Specification Handle for Enemy Stats Missing!"))
		return;
	}
	
	// Get all Associated Tags
	FGameplayTag ArmourTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.Armour"));
	FGameplayTag BioMatterTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.BioMatter"));
	FGameplayTag CurrentHealthTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.CurrentHealth"));
	FGameplayTag MaxHealthTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.MaxHealth"));
	FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.Damage"));
	FGameplayTag NuclearTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.NuclearMatter"));
	FGameplayTag ChitinTag = FGameplayTag::RequestGameplayTag(FName("Stats.Base.ChitinChance"));
	
	// Assign Magnitudes for each Tag
	SpecHandle.Data->SetSetByCallerMagnitude(ArmourTag, NewArmour);
	SpecHandle.Data->SetSetByCallerMagnitude(BioMatterTag, NewBioMatter);
	SpecHandle.Data->SetSetByCallerMagnitude(CurrentHealthTag, NewHealth);
	SpecHandle.Data->SetSetByCallerMagnitude(MaxHealthTag, NewHealth);
	SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, NewDamage);
	SpecHandle.Data->SetSetByCallerMagnitude(NuclearTag, NewNuclearChance);
	SpecHandle.Data->SetSetByCallerMagnitude(ChitinTag, NewChitinChance);
	
	// Apply Effect
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AEnemyBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Call the blueprint Version of the function
	OnHealthChanged(Data.OldValue, Data.NewValue);
	
	// If the AttackGroup is valid
	if (IsValid(AttackGroup))
	{
		// Set Triggered to true if not already (This should also Alert all enemies)
		if (!AttackGroup->bTriggered)
		{
			AttackGroup->bTriggered = true;
		}
	}
	
	// If health is zero
	if (Data.NewValue == 0)
	{
		// Call Death Functionality (NOT DESTROY)
		if (IDDEnemyEventManager* DDGameState = Cast<IDDEnemyEventManager>(GetWorld()->GetGameState()))
		{
			DDGameState->Publish_EnemyDeath(this);
		}
		// Call Pre-Death, to nullify enemy behaviours before deletion
		PreDeath();
	}
}

void AEnemyBase::OnKnockedTagChanged(const struct FGameplayTag CallbackTag, int32 NewCount)
{
	// if the New Number of Knocked Tags is 0 (Knocked was removed)
	if (NewCount == 0)
	{
		// Stand the Character up
		StandUp();
	}
}

bool AEnemyBase::TryAttack(AActor* TargetActor)
{
	// Double Check Conditions of the TargetActor and return wether its possible for this enemy to attack correctly.

	/*
		This should consider, in order:
			AttackCooldown
			Validity of Self and Target
	 */
	// If the Enemy already has the Cooldown tag
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Enemy.Attacking.Cooldown"))))
	{
		// Cannot Attack, on Cooldown
		return false;
	}

	// If either This enemy or the Target Actor are Invalid
	if (!IsValid(this) || !IsValid(TargetActor))
	{
		// Attack cannot take place, due to being invalid
		return false;
	}

	// If nothing was hit earlier, the attack can go ahead
	AttackStart(TargetActor);
	return true;
}

void AEnemyBase::CommitAttack(AActor* TargetActor)
{
	// Commit the Attack, remove the "Active" tag 
	// AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Enemy.Attacking.Active")));
	
	// Do a final check for whether the attack can actually succeed or not
	// Assuming melee attack
	TArray<UShapeComponent*> TargetHitBoxes;
	TSet<UActorComponent*> TargetComponents = TargetActor->GetComponents();
	for (auto Component : TargetComponents)
	{
		// If the component is a ShapeComponent
		if (UShapeComponent* HitBoxComponent = Cast<UShapeComponent>(Component))
		{
			// And that Shape Component is a HitBox
			if (HitBoxComponent->GetCollisionObjectType() == ECollisionChannel::ECC_GameTraceChannel2)
			{
				// Save the Hitbox Component for further processing
				TargetHitBoxes.Add(HitBoxComponent);
			}
		}
	}

	// If we successfully detected one or more TargetHitBoxes
	for (auto TargetHitBox : TargetHitBoxes)
	{
		// Check if the HurtBox is overlapping the TargetHitBox
		if (HurtBox->IsOverlappingComponent(TargetHitBox))
		{
			// Call Successful Hit, process damage in BP
			OnAttackSuccess(TargetActor, TargetHitBox);
			return;
		}
	}

	// Call Failed Hit, do some other effect
	OnAttackFailed(TargetActor);
}

void AEnemyBase::StandUp()
{
	// Change Collision types and reset physics
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetAllPhysicsLinearVelocity(FVector(0));
	
	// Set random rotation
	SetActorRotation(FRotator(0, FMath::FRandRange(0.f, 359.f), 0));
	
	// Call the BP Event for this function
	OnStandUp();
}

void AEnemyBase::SetOwningAttackGroup(class AAttackGroup* NewAttackGroup)
{
	AttackGroup = NewAttackGroup;
}

AAttackGroup* AEnemyBase::GetOwningAttackGroup() const
{
	return AttackGroup;
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Set AbilitySystem stats if ABSC is valid
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Perform initialisation of stats
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitialiseAbilities();
	InitialiseEffects();
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Subscribe to the Enemy Death Delegate
	if (ADustDevilGameState* DDGameState = Cast<ADustDevilGameState>(GetWorld()->GetGameState()))
	{
		DDGameState->OnDeathDelegate.AddUniqueDynamic(AttackGroup, &AAttackGroup::RemoveEnemyFromGroup);
	}
	
	// Assign a Delegate for Health Changing
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AbilitySystemComponent->GetSet<UBaseAttributeSet>()->GetCurrentHealthAttribute()).AddUObject(this, &AEnemyBase::OnHealthAttributeChanged);

	// Assign Delegate for Knocked Tag Removal (Knockback correcting)
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("Event.Enemy.Knocked"), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemyBase::OnKnockedTagChanged);
}

void AEnemyBase::Instakill()
{
	// Set health to zero
	AbilitySystemComponent->SetNumericAttributeBase(UBaseAttributeSet::GetCurrentHealthAttribute(), 0);
}

void AEnemyBase::PreDeath()
{
	// Turn off functionality to the enemy.

	// Add a Tag to notify Death, this prevents the AI from functioning and also notifies the Animation graph to play a death animation
	FGameplayTagContainer TagContainer = AbilitySystemComponent->GetOwnedGameplayTags();
	FGameplayTagContainer TagsToRemove;
	// For every tag inside this enemy
	for (auto Tag : TagContainer)
	{
		// if the tag comes under the State.Enemy hierarchy
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Enemy")))
		{
			// Add to Remove list
			TagsToRemove.AddTag(Tag);
		}
	}
	// Remove safely
	for (auto Tag : TagsToRemove)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
	}
	
	// Then Place Death tag afterward
	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Enemy.Death"));

	// Deactivate Collision Hit and HurtBoxes
	HitBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	HurtBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	// Then Call OnDeath, so other functionality can happen
	OnDeath();
}

void AEnemyBase::AttackStart(AActor* TargetActor)
{
	// Currently Remove Other State Tags and add the Attacking Tag
	// Get List of all tags under category "State.Enemy", then exclude ones with the "Cooldown" suffix.
	FGameplayTagContainer AllOwnedTags = AbilitySystemComponent->GetOwnedGameplayTags();
	FGameplayTagContainer TagsToRemove;
	for (auto Tag : AllOwnedTags)
	{
		// If tag is "State.Enemy.x" and is not specifically "State.Enemy.x.Cooldown"
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Enemy")) && !Tag.GetTagName().ToString().Contains("Cooldown", ESearchCase::IgnoreCase, ESearchDir::FromEnd))
		{
			// Add to list of Tags to remove
			TagsToRemove.AddTag(Tag);
		}
	}
	// Remove the Tags to Remove from the Gameplay Ability System
	AbilitySystemComponent->RemoveLooseGameplayTags(TagsToRemove);
	
	// Add a new tag for Attacking.Active
	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Enemy.Attacking.Active"));

	// Call the BP OnAttack
	OnAttackStart(TargetActor);
}
