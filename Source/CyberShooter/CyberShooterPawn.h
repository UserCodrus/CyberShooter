// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Breakable.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CyberShooterPawn.generated.h"

class UWeapon;
class UAbility;

// A player or enemy character
UCLASS(BlueprintType, Blueprintable)
class ACyberShooterPawn : public APawn, public IBreakable
{
	GENERATED_BODY()

public:
	ACyberShooterPawn();

	virtual void BeginPlay() override;

	/// Breakable Interface ///
	
	virtual void Damage(int32 Value, int32 DamageType, AActor* Source = nullptr, AActor* Origin = nullptr);
	virtual void Impulse(FVector Force);
	virtual void Heal(int32 Value);
	virtual void Kill();

	// Stop firing and using abilities
	void StopAction();

	/// Accessor Functions ///
	
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

	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return MeshComponent; }
	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMomentum() const { return Momentum; }
	FORCEINLINE float GetMaxMomentum() const { return MaxMomentum; }
	FORCEINLINE float GetTickSpeed() const { return TickSpeed; }

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
	// Manage the momentum drain of the pawn's ability
	void SustainAbility(float DeltaTime);

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
	// The speed muliplier at max momentum
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere, BlueprintReadWrite)
		float MomentumBonus;

	// The momentum reward for killing this pawn
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere, BlueprintReadWrite)
		float MomentumReward;
	// The momentum lost upon taking damage
	UPROPERTY(Category = "Attributes|Combat", EditAnywhere, BlueprintReadWrite)
		float MomentumPenalty;

	// Flags for which types of damage can affect the pawn
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;
	// The direction from which the pawn can be damaged
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		FVector DamageDirection;
	// The minimum damage angle
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		float MinimumDamageAngle;
	// The maximum damage angle
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
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

	// The forward vector for the player
	UPROPERTY(Category = "Movement|Orientation", EditAnywhere, BlueprintReadWrite)
		FVector Forward;
	// The up vector for the player
	UPROPERTY(Category = "Movement|Orientation", EditAnywhere, BlueprintReadWrite)
		FVector Up;

	// The force mutiplier for physics collisions
	UPROPERTY(Category = "Movement|Physics", EditAnywhere, BlueprintReadWrite)
		float CollisionForce;

	// The tick speed of the pawn
	UPROPERTY(Category = "Time", EditAnywhere, BlueprintReadWrite)
		float TickSpeed;

	/// Components ///

	// The collision capsule
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		class UCapsuleComponent* CollisionComponent;
	// The character's static mesh
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* MeshComponent;

	// Particles that spawn on death
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadOnly)
		UParticleSystem* DeathParticles;
	// the sound that plays on death
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadOnly)
		USoundBase* DeathSound;

	float ShotCooldown;
	float AbilityCooldown;
};

