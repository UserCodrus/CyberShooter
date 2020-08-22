// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterProjectile.h"
#include "CyberShooterPawn.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

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

	InitialLifeSpan = 3.0f;
	NumBounces = 0;
	Damage = 0.0f;
	Force = 20000.0f;
}

void ACyberShooterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACyberShooterPawn* target = Cast<ACyberShooterPawn>(OtherActor);

	// Handle collisions
	if (OtherActor != nullptr && OtherActor != this)
	{
		// Collide with pawns
		if (target != nullptr)
		{
			target->ChangeHealth(-Damage);
		}

		// Collide with physics objects
		if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetActorRotation().Vector() * Force, GetActorLocation());
		}
	}

	// Play the impact sound
	if (ImpactSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	
	// Bounce if possible
	if (NumBounces > 0)
	{
		if (BounceOnPawn || target == nullptr)
		{
			NumBounces--;
			return;
		}
	}

	// Create explosion particles
	if (DestructionParticles != nullptr)
	{
		FTransform transform;
		transform.SetLocation(GetActorLocation());
		transform.SetRotation(GetActorRotation().Quaternion());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionParticles, transform);
	}

	// Destroy the projectile
	Destroy();
}