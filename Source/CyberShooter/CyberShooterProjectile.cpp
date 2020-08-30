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
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.0f;
	ProjectileMovement->Friction = 0.0f;

	InitialLifeSpan = 3.0f;
	NumBounces = 0;
	BounceOnPawn = false;
	ExplosionRadius = 0.0f;
	HitOnBounce = true;
	Damage = 0.0f;
	DamageType = 0;
	Force = 10000.0f;

	Source = nullptr;
}

void ACyberShooterProjectile::SetSource(AActor* ProjectileSource)
{
	if (Source == nullptr)
	{
		Source = ProjectileSource;
	}
}

void ACyberShooterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Handle collisions
	if (OtherActor != nullptr && OtherActor != this)
	{
		// Bounce if possible
		if (NumBounces != 0)
		{
			if (BounceOnPawn || Cast<ACyberShooterPawn>(OtherActor) == nullptr)
			{
				// Apply damage and physics
				if (HitOnBounce)
				{
					ApplyImpact(OtherActor, OtherComp);
				}

				// Play the bounce sound
				if (BounceSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
				}

				// Create bounce particles
				if (BounceParticles != nullptr)
				{
					FTransform transform;
					transform.SetLocation(GetActorLocation());
					transform.SetRotation(GetActorRotation().Quaternion());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BounceParticles, transform);
				}

				NumBounces--;
				return;
			}
		}

		// Apply damage
		ApplyImpact(OtherActor, OtherComp);

		// Play the destruction sound
		if (DestructionSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DestructionSound, GetActorLocation());
		}

		// Create destruction particles
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
}

void ACyberShooterProjectile::ApplyImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (ExplosionRadius <= 0.0f)
	{
		// Deal damage to breakables
		IBreakable* target = Cast<IBreakable>(OtherActor);
		if (target != nullptr)
		{
			target->Damage(Damage, DamageType, this, Source);
		}

		// Apply physics
		if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity().GetSafeNormal() * Force, GetActorLocation());
		}
	}
	else
	{
		// Get all actors within the explosion radius
		TArray<TEnumAsByte<EObjectTypeQuery>> types;
		TArray<AActor*> ignore;
		TArray<AActor*> actors;
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), ExplosionRadius, types, AActor::StaticClass(), ignore, actors);

		// Affect every actor in range of the explosion
		for (int32 i = 0; i < actors.Num(); ++i)
		{
			// Damage breakables
			IBreakable* target = Cast<IBreakable>(actors[i]);
			if (target != nullptr)
			{
				target->Damage(Damage, DamageType, this, Source);
			}

			// Apply physics
			UPrimitiveComponent* component = Cast<UPrimitiveComponent>(actors[i]->GetRootComponent());
			if (component != nullptr && component->IsSimulatingPhysics())
			{
				FVector direction = actors[i]->GetActorLocation() - GetActorLocation();
				component->AddImpulseAtLocation(direction.GetSafeNormal() * Force, GetActorLocation());
			}
		}
	}
}