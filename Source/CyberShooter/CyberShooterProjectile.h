// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberShooterProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;
class USphereComponent;

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
	// The movement component for the projectile
	UPROPERTY(Category = Movement, VisibleAnywhere, BlueprintReadOnly)
		UProjectileMovementComponent* ProjectileMovement;

	// The particle system to spawn when the projectile breaks
	UPROPERTY(Category = Projectile, EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* DestructionParticles;
};

