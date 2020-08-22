// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

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

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns MeshComponent subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return MeshComponent; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	// The projectile's collision
	UPROPERTY(Category = Projectile, VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* CollisionComponent;
	// The mesh for the projectile
	UPROPERTY(Category = Projectile, VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshComponent;
	// The particle trail for the projectile
	UPROPERTY(Category = Projectile, VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* ParticleSystem;
	// The particle system to spawn when the projectile breaks
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* DestructionParticles;

	// The movement component for the projectile
	UPROPERTY(Category = Movement, VisibleAnywhere, BlueprintReadOnly)
		UProjectileMovementComponent* ProjectileMovement;

	// The sound played when hitting an object
	UPROPERTY(Category = Sound, EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* ImpactSound;
	
	// The number of times the projectile can bounce
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		int32 NumBounces;
	// If set to true, the projectile will bounce off of pawns as well as the environment
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		bool BounceOnPawn;
	// The damage the projectile does to breakables and enemies
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		int32 Damage;
	// The strength of projectile impacts of physics objects
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		float Force;
};

