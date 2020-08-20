// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"

ACyberShooterProjectile::ACyberShooterProjectile() 
{
	// Create the collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision0"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterProjectile::OnHit);
	RootComponent = CollisionComponent;

	// Create mesh component for the projectile sphere
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->SetupAttachment(RootComponent);

	// Create the particle system
	ParticleSystem = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("ProjectileParticles0"));
	ParticleSystem->SetAbsolute(false, false, true);
	ParticleSystem->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.0f;
	ProjectileMovement->Friction = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	NumBounces = 0;
	ImpactForce = 20.0f;
}

void ACyberShooterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * ImpactForce, GetActorLocation());
	}
	
	if (NumBounces > 0)
	{
		// Reduce the bounce counter
		NumBounces--;
	}
	else
	{
		// Create explosion particles
		FTransform transform;
		transform.SetLocation(GetActorLocation());
		transform.SetRotation(GetActorRotation().Quaternion());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionParticles, transform);

		// Destroy the projectile
		Destroy();
	}
}