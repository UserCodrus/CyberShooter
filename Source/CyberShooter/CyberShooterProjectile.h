// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Breakable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberShooterProjectile.generated.h"

class UBulletMovementComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;
class USphereComponent;
class USoundBase;

UCLASS(config=Game)
class ACyberShooterProjectile : public AActor
{
	GENERATED_BODY()

public:
	ACyberShooterProjectile();

	// Mark what spawned the projectile
	UFUNCTION()
		void SetSource(AActor* ProjectileSource);
	// Function to handle the projectile hitting something
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Change the air friction of the projectile
	void SetAirFriction(float NewFriction);
	// Reset air friction to the default value in the game instance
	void ResetAirFriction();
	// Change the tick speed of the movement component
	void SetTickSpeed(float NewSpeed);
	// Reset tick speed to its default value
	void ResetTickSpeed();
	// Change the static forces applied to the bullet
	void SetStaticForce(FVector NewForce);
	// Reset static force to its default value
	void ResetStaticForce();

	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return MeshComponent; }
	FORCEINLINE UBulletMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	// Apply damage and physics effects
	void ApplyImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	// The projectile's collision
	UPROPERTY(Category = "Components", VisibleAnywhere)
		USphereComponent* CollisionComponent;
	// The mesh for the projectile
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UStaticMeshComponent* MeshComponent;
	// The particle trail for the projectile
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UParticleSystemComponent* ParticleSystem;

	// The movement component for the projectile
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UBulletMovementComponent* ProjectileMovement;

	// The particle system to spawn when the projectile breaks
	UPROPERTY(Category = "Effects|Particles", EditDefaultsOnly)
		UParticleSystem* DestructionParticles;
	// The particle system to spawn when bouncing off of a surface
	UPROPERTY(Category = "Effects|Particles", EditDefaultsOnly)
		UParticleSystem* BounceParticles;

	// The sound played when the bullet bounces
	UPROPERTY(Category = "Effects|Sound", EditDefaultsOnly)
		USoundBase* BounceSound;
	// The sound played when the bullet is destroyed
	UPROPERTY(Category = "Effects|Sound", EditDefaultsOnly)
		USoundBase* DestructionSound;
	
	// The number of times the projectile can bounce
	UPROPERTY(Category = "Projectile|Bounce", EditDefaultsOnly)
		int32 NumBounces;
	// If set to true, the projectile will bounce off of pawns as well as the environment
	UPROPERTY(Category = "Projectile|Bounce", EditDefaultsOnly)
		bool BounceOnPawn;
	// If set to true, each bounce will count as a hit for dealing damage or exploding
	UPROPERTY(Category = "Projectile|Bounce", EditDefaultsOnly)
		bool HitOnBounce;
	
	// The damage the projectile does to breakables and enemies
	UPROPERTY(Category = "Projectile|Damage", EditDefaultsOnly)
		int32 Damage;
	// The damage type of the projectile
	UPROPERTY(Category = "Projectile|Damage", EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageType;
	// The radius of projectile explosions
	UPROPERTY(Category = "Projectile|Damage", EditDefaultsOnly)
		float ExplosionRadius;
	// The strength of projectile impacts of physics objects
	UPROPERTY(Category = "Projectile|Damage", EditDefaultsOnly)
		float Force;
	// The strength of projectile impacts on players and enemies
	UPROPERTY(Category = "Projectile|Damage", EditDefaultsOnly)
		float Impulse;

	// the actor that spawned this projectile
	UPROPERTY(VisibleInstanceOnly)
		AActor* Source;
};

