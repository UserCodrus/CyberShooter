// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Breakable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberShooterProjectile.generated.h"

class UProjectileMovementComponent;
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

	/** Returns MeshComponent subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return MeshComponent; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	// Apply damage and physics effects
	void ApplyImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	// The projectile's collision
	UPROPERTY(Category = Components, VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* CollisionComponent;
	// The mesh for the projectile
	UPROPERTY(Category = Components, VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshComponent;
	// The particle trail for the projectile
	UPROPERTY(Category = Components, VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* ParticleSystem;

	// The movement component for the projectile
	UPROPERTY(Category = Movement, VisibleAnywhere, BlueprintReadOnly)
		UProjectileMovementComponent* ProjectileMovement;

	// The particle system to spawn when the projectile breaks
	UPROPERTY(Category = Particles, EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* DestructionParticles;
	// The particle system to spawn when bouncing off of a surface
	UPROPERTY(Category = Particles, EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* BounceParticles;

	// The sound played when the bullet bounces
	UPROPERTY(Category = Sound, EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* BounceSound;
	// The sound played when the bullet is destroyed
	UPROPERTY(Category = Sound, EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* DestructionSound;
	
	// The number of times the projectile can bounce
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		int32 NumBounces;
	// If set to true, the projectile will bounce off of pawns as well as the environment
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		bool BounceOnPawn;
	// If set to true, each bounce will count as a hit for dealing damage or exploding
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		bool HitOnBounce;
	// The radius of projectile explosions
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		float ExplosionRadius;
	
	// The damage the projectile does to breakables and enemies
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		int32 Damage;
	// The damage type of the projectile
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EDamageType))
		int32 DamageType;
	// The strength of projectile impacts of physics objects
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		float Force;

	// the actor that spawned this projectile
	UPROPERTY(VisibleInstanceOnly)
		AActor* Source;
};

