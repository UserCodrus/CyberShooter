// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CyberShooterPawn.generated.h"

class UWeapon;
class UAbility;

// A player or enemy character
UCLASS(BlueprintType, Blueprintable)
class ACyberShooterPawn : public APawn, public ICombatInterface
{
	GENERATED_BODY()

public:
	ACyberShooterPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/// ICombatInterface ///
	
	virtual void Damage(int32 Value, int32 DamageType, AActor* Source = nullptr, AActor* Origin = nullptr) override;
	virtual void Heal(int32 Value) override;
	virtual void Kill() override;

	/// Accessor Functions ///

	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMomentum() const { return Momentum; }
	FORCEINLINE float GetMaxMomentum() const { return MaxMomentum; }
	FORCEINLINE float GetMomentumBlockSize() const { return MomentumBlockSize; }
	FORCEINLINE float GetTickSpeed() const { return TickSpeed; }

	// Stop firing and using abilities
	void StopAction();

	// Get the forward vector of the pawn's current orientation
	FVector GetForwardVector() const;
	// Get the up vector of the pawn's current orientation
	FVector GetUpVector() const;
	// Get the rotator corresponding to the pawn's orientation
	FRotator GetOrientationRotator() const;
	// Get the forward vector of the pawn's core component
	FVector GetCoreForwardVector() const;
	// Get the up vector of the pawn's core component
	FVector GetCoreUpVector() const;
	// Get the rotation of the pawn's core component in world space
	FRotator GetCoreRotation() const;
	// Get the rotation of the pawn's core component in local space
	FRotator GetCoreLocalRotation() const;

	// Set the rotation of the core component in local space
	void SetCoreRotation(FRotator Rotation);
	void SetCoreRotation(FQuat Rotation);
	// Set the rotation of the core component in world space
	void SetCoreWorldRotation(FRotator Rotation);
	void SetCoreWorldRotation(FQuat Rotation);

	// Add or remove momentum from the pawn
	UFUNCTION(BlueprintCallable)
		void ChangeMomentum(float Value);
	// Add environmental damage over time to the pawn
	UFUNCTION(BlueprintCallable)
		void SetEnvironmentDamage(float Damage);
	// Change the tick speed of the pawn
	UFUNCTION(BlueprintCallable)
		virtual void SetTickSpeed(float NewSpeed);

	// Activate the pawn's weapon
	void StartFiring();
	// Deactivate the pawn's weapon
	void StopFiring();

protected:
	// Activate the current ability
	void StartAbility();
	// Deactivate the current ability
	void StopAbility();

	// Call the activate script for the current ability
	bool ActivateAbility();
	// Call the deactivate script for the current ability
	bool DeactivateAbility();

	// Fire the pawn's current weapon
	void Fire(FVector FireDirection);

	/// Attributes ///
	
	// The maximum health of the pawn
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere, BlueprintReadWrite)
		int32 MaxHealth;
	// The pawn's current health
	UPROPERTY(Category = "Attributes|Combat", EditInstanceOnly, BlueprintReadWrite)
		int32 Health;
	// The maximum momentum for the pawn
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere, BlueprintReadWrite)
		float MaxMomentum;
	// The pawn's current momentum
	UPROPERTY(Category = "Attributes|Combat", EditInstanceOnly, BlueprintReadWrite)
		float Momentum;
	// The speed muliplier added for each full block of momentum
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere)
		float MomentumBonus;

	// The momentum reward for killing this pawn
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere)
		float MomentumReward;
	// The size of momentum blocks, for UI display and upgrades
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere)
		float MomentumBlockSize;
	// The momentum this pawn loses when taking damage, relative to block size
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere)
		float MomentumPenalty;

	// Flags for which types of damage can affect the pawn
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;
	// The direction from which the pawn can be damaged
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere)
		FVector DamageDirection;
	// The duration the pawn will blink after taking damage
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere)
		float DamageCooldownDuration;
	// The minimum damage angle
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere)
		float MinimumDamageAngle;
	// The maximum damage angle
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere)
		float MaximumDamageAngle;

	// Damage the pawn is currently taking from its environment
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		float EnvironmentDamage;

	/// Weapons ///

	// The currently equipped weapon
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		UWeapon* Weapon;
	// The offset for spawning projectiles
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		float GunOffset;
	// Set to true when the pawn is firing its weapon
	UPROPERTY(Category = "Weapon", VisibleAnywhere, BlueprintReadOnly)
		bool FireWeapon;

	// The currently equipped ability
	UPROPERTY(Category = "Ability", EditAnywhere, BlueprintReadWrite)
		UAbility* Ability;
	// Set to true when the pawn is using its ability
	UPROPERTY(Category = "Ability", VisibleAnywhere, BlueprintReadOnly)
		bool UseAbility;

	/// Movement ///

	// The force mutiplier for physics collisions
	UPROPERTY(Category = "Movement|Physics", EditAnywhere, BlueprintReadWrite)
		float CollisionForce;

	// The tick speed of the pawn
	UPROPERTY(Category = "Time", EditAnywhere, BlueprintReadWrite)
		float TickSpeed;

	/// Components ///

	// The core component that stores local transformations for the pawn
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		USceneComponent* CoreComponent;
	// The collision capsule
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		class UCapsuleComponent* CollisionComponent;

	// Particles that spawn on death
	UPROPERTY(Category = "Effects", EditAnywhere)
		UParticleSystem* DeathParticles;
	// the sound that plays on death
	UPROPERTY(Category = "Effects", EditAnywhere)
		USoundBase* DeathSound;
	// The force feedback that plays on death
	UPROPERTY(Category = "Effects", EditAnywhere)
		UForceFeedbackEffect* DeathRumble;

	float ShotCooldown;
	float AbilityCooldown;
	float DamageCooldown;
};

