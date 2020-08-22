// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Weapon.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CyberShooterPawn.generated.h"

// A player or enemy character
UCLASS(Blueprintable)
class ACyberShooterPawn : public APawn
{
	GENERATED_BODY()

public:
	ACyberShooterPawn();

	// Function for changing health
	UFUNCTION()
		void ChangeHealth(float Value);
	// Called when the pawn is killed
	UFUNCTION()
		virtual void Kill();
	// The function that handles the ship hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Activate the pawn's weapon
	void StartFiring();
	// Deactivate the pawn's weapon
	void StopFiring();
	// Fire the pawn's current weapon
	void FireShot(FVector FireDirection);
	// Callback for when the shot timer expires
	void ShotTimerExpired();

	// Accessor functions
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return MeshComponent; }

protected:
	/// Attributes ///
	
	// The maximum health of the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MaxHealth;
	// The pawn's current health
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		float Health;
	// The maximum momentum for the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MaxMomentum;
	// The pawn's current momentum
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		float Momentum;
	// The speed muliplier at max momentum
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		float MomentumBonus;

	/// Weapons ///

	// The currently equipped weapon
	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite)
		UWeapon* CurrentWeapon;
	// The offset for spawning projectiles
	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite)
		float GunOffset;
	// Set to true when the pawn is firing its weapon
	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadWrite)
		bool FireWeapon;

	// Set to true when the pawn is able to fire its weapon
	bool CanFire;

	/// Movement ///

	// The speed that the ship moves
	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;
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


	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
};

