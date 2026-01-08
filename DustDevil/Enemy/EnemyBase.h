// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DustDevil/Interfaces/DDIDamageable.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

enum EEnemyType : uint8;
enum EEnemyTier : uint8;

UCLASS()
class DUSTDEVIL_API AEnemyBase : public ACharacter, public IAbilitySystemInterface, public IDDIDamageable
{
	GENERATED_BODY()
	
	/*
	Functions:
		Die (Rework this to be in stages, functional death, then actor death)
		CreateMinimapMarkers
		AttackPlayer
		
		Potentially Deprecated:
			StartListening
			StopListening
			SetDefaultStats
			FeedbackUI (Replace with GameplayCues)
			EnemyHitFlash
			EnemyDeath
			EnemySquadEliminated
			EventReceiveDamage
			TimerCounting
			ProjectileDamage
			ChangeColourBasedOnDamage
			EnemyDamaged
			DamageFeedback
			EnemyFeedback
			FinishAttackAnim
			RemoveFromMinimap
			Rammed
			Squish
			ReceiveDamage		

	 Variables:

		Potentially Deprecated
			AttackRange, Distance, Damage and Cooldown
			DetectionRadius
			References
			Async
			bInGroup
			OwningAttackGroupIndex
			Health
			bShouldMove
			BioMatterDropped
			Material (hit flash)
			all Anim Data
			EnemySpeed	
	 */
	
public:
	// More Data Associated with the Specifics of this enemy.

	// The Tier of enemy, used as a secondary identifier and allows exclusion from spawning in attack groups
	UPROPERTY(BlueprintReadWrite, Category = "Enemy")
	TEnumAsByte<EEnemyTier> Tier;

	// The Type of enemy, used as a tertiary identifier and allows filtering in attack group generation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (Bitmask="EnemyTypes", BitmaskEnum = "/Script/DustDevil.EEnemyTypes"))
	int32 Types;

	// The Range at which the Enemy will attempt to attack the player (NOT the range of the Attack Itself)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float TryAttackRange = 100.f;

	// The Rate at which the enemy can attack, (1 attack per this many seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackCooldown = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|GAS")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;
	
public:
	// Sets default values for this character's properties
	AEnemyBase();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DrawDebugSymbols(float DeltaTime);
	
	// Override Interface Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Override Damageable Interface
	virtual int32 GetDamageableType_Implementation() override;
	
	// Overridable function for Ability Initialization
	virtual void InitialiseAbilities();
	// Overridable function for Effect Initialization
	virtual void InitialiseEffects();
	// Function Called within the Enemy Spawn Function, Allocates Data pertaining to the Enemy, PRIOR to BeginPlay()
	void ApplyInitialAttributes(TSubclassOf<class UGameplayEffect> EffectClass, float NewHealth, float NewArmour, float NewDamage, float NewBioMatter, float NewNuclearChance, float NewChitinChance);

	// Intermediate step for delegate calling, Called when health Value changes
	virtual void OnHealthAttributeChanged(const struct FOnAttributeChangeData& Data);

	// Blueprint Function, Called every single time the health value changes (Up or Down)
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|GAS")
	void OnHealthChanged(float OldValue, float NewValue);

	// Called when the enemies knocked tag changes
	void OnKnockedTagChanged(const struct FGameplayTag CallbackTag, int32 NewCount);

	// Attempt to attack the Specified Target Actor
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	bool TryAttack(AActor* TargetActor);

	// Blueprint function, called when the Enemy successfully attempts to attack (Animation Started). This should set GAS data, such as the attack cooldown
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Combat")
	void OnAttackStart(AActor* TargetActor);

	// Trigger explicitly when the animation allows for the attack to occur
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void CommitAttack(AActor* TargetActor);

	// Blueprint Event, Called when CommitAttack succeeds
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Combat")
	void OnAttackSuccess(AActor* TargetActor, UShapeComponent* HitComponent);

	// Blueprint Event, Called when Commit Attack Fails.
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Combat")
	void OnAttackFailed(AActor* TargetActor);
	
	// Cause the Enemy to 'Stand Up', ie, undo knockback effect and become movable again
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void StandUp();

	// Blueprint function, called after the enemy 'rights' itself after being knocked over
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnStandUp();

	// Set the owning attack group
	void SetOwningAttackGroup(class AAttackGroup* NewAttackGroup);

	// Gets the Owning attack group, should only need to be used for assigning to a Blackboard
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	class AAttackGroup* GetOwningAttackGroup() const;
	
	// Initialize the AbilitySystemComponent Before BeginPlay() but after ApplyInitialAttributes().
	virtual void PostInitializeComponents() override;
	
protected:
	// GAS
	UPROPERTY(VisibleAnywhere, Category = "Enemy|GAS")
	class UDD_AbilitySystemComponent* AbilitySystemComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sets this Enemies health to zero
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void Instakill();
	
	// Called when the enemies health hits zero, sets death tags for animations and item drops to play out. This function should delete the actor after a delay.
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void PreDeath();

	// Is Called AFTER Pre-Death, This function can safely destroy the Enemy (but should only do so after the animation has finished)
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDeath();

private:
	/*
		What do the Enemies Actually need?
		RootComponent
			Capsule Component for environment Collisions		(Default)
			Arrow Component										(Default)
			Skeletal mesh Component								(Default)
				MeleeHurtBox (can be sphere)					(Vital)
			HitBox (To be hit)									(Vital)
	
		General Components
			 CharacterMovementComponent							(Default)
			 AbilitySystemComponent								(Vital)
	*/
	
	// The Hitbox, the area in which the enemy must be Hit in order to take damage (As an AABB)
	// HitBox Z Extent should be 512 (1024 length along Z)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = true))
	class UBoxComponent* HitBox;
	
	// The HurtBox, the area by which the enemy Deals damage (Sphere) 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = true))
	class USphereComponent* HurtBox;

private:
	// The Owning AttackGroup, this is EXPLICITLY for blackboard declaration, since AIControllers are so greedy
	UPROPERTY()
	class AAttackGroup* AttackGroup;

private:
	// c++ functionality pertaining to an Attack, calls the OnAttack Function
	void AttackStart(AActor* TargetActor);

};
