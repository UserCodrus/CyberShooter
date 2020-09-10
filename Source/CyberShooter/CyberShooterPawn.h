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
	
	virtual void Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin);
	virtual void Heal(int32 Value);
	virtual void Kill();

	// The function that handles the ship hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Stop firing and using abilities
	void StopAction();
	// Callback for when the shot timer expires
	void EndShotCooldown();
	// Call back for when the ability timer expires
	void EndAbilityCooldown();

	/// Accessor Functions ///
	
	void ChangeMomentum(float Value);

	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return MeshComponent; }
	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMomentum() const { return Momentum; }
	FORCEINLINE float GetMaxMomentum() const { return MaxMomentum; }

protected:
	// Activate the pawn's weapon
	void StartFiring();
	// Deactivate the pawn's weapon
	void StopFiring();
	// Activate the current ability
	void StartAbility();
	// Deactivate the current ability
	void StopAbility();

	// Call the activate script for the current ability
	bool ActivateAbility();
	// Call the deactivate script for the current ability
	bool DeactivateAbility();

	// Fire the pawn's current weapon
	void FireShot(FVector FireDirection, FVector CenterAxis = FVector(0.0f, 0.0f, 1.0f));
	// Manage the momentum drain of the pawn's ability
	void SustainAbility(float DeltaTime);

	/// Attributes ///
	
	// The maximum health of the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		int32 MaxHealth;
	// The pawn's current health
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		int32 Health;
	// The maximum momentum for the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MaxMomentum;
	// The pawn's current momentum
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		float Momentum;
	// The speed muliplier at max momentum
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MomentumBonus;

	// The momentum reward for killing this pawn
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MomentumReward;
	// The momentum lost upon taking damage
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MomentumPenalty;

	// Flags for which types of damage can affect the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageType))
		int32 DamageImmunity;
	// The direction from which the pawn can be damaged
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		FVector DamageDirection;
	// The minimum damage angle
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MinimumDamageAngle;
	// The maximum damage angle
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MaximumDamageAngle;

	/// Weapons ///

	// The currently equipped weapon
	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite)
		UWeapon* Weapon;
	// The offset for spawning projectiles
	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite)
		float GunOffset;
	// Set to true when the pawn is firing its weapon
	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly)
		bool FireWeapon;

	// The currently equipped ability
	UPROPERTY(Category = Ability, EditAnywhere, BlueprintReadWrite)
		UAbility* Ability;
	// Set to true when the pawn is using its ability
	UPROPERTY(Category = Ability, VisibleAnywhere, BlueprintReadOnly)
		bool UseAbility;

	/// Movement ///

	// The force mutiplier for physics collisions
	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadWrite)
		float CollisionForce;

	/// Components ///

	// The collision capsule
	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly)
		class UCapsuleComponent* CollisionComponent;
	// The character's static mesh
	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* MeshComponent;

	// Particles that spawn on death
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		UParticleSystem* DeathParticles;
	// the sound that plays on death
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		USoundBase* DeathSound;

	// Set to true when the pawn is able to fire its weapon
	bool CanFire;
	// Set to true when the pawn is able to use its ability
	bool CanUseAbility;

	// Handle for shot cooldown
	FTimerHandle TimerHandle_ShotCooldown;
	// Handle for ability cooldowns
	FTimerHandle TimerHandle_AbilityCooldown;
};

