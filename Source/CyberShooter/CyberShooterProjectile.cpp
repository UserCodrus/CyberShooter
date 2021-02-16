// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterProjectile.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"
#include "BulletMovementComponent.h"
#include "PhysicsInterface.h"
#include "CombatInterface.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
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
	ProjectileMovement = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;

	InitialLifeSpan = 3.0f;
	NumBounces = 0;
	BounceOnPawn = false;
	ExplosionRadius = 0.0f;
	HitOnBounce = true;
	Damage = 0.0f;
	DamageType = DAMAGETYPE_NONE;
	Impulse = 500.0f;

	Source = nullptr;
}

void ACyberShooterProjectile::SetSource(AActor* ProjectileSource)
{
	if (Source == nullptr)
	{
		Source = ProjectileSource;
		//ProjectileMovement->AddVelocity(ProjectileSource->GetVelocity());
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

void ACyberShooterProjectile::SetAirFriction(float NewFriction)
{
	ProjectileMovement->SetAirFriction(NewFriction);
}

void ACyberShooterProjectile::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		ProjectileMovement->SetAirFriction(instance->GetAirFriction());
	}
}

void ACyberShooterProjectile::SetTickSpeed(float NewSpeed)
{
	ProjectileMovement->SetTickSpeed(NewSpeed);
}

void ACyberShooterProjectile::ResetTickSpeed()
{
	ProjectileMovement->SetTickSpeed(1.0f);
}

void ACyberShooterProjectile::SetStaticForce(FVector NewForce)
{
	ProjectileMovement->SetStaticForce(NewForce);
}

void ACyberShooterProjectile::ResetStaticForce()
{
	ProjectileMovement->SetStaticForce(FVector(0.0f));
}

void ACyberShooterProjectile::ApplyImpact(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (ExplosionRadius <= 0.0f)
	{
		// Apply physics
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			ACyberShooterPawn* pawn = Cast<ACyberShooterPawn>(OtherActor);
			if (pawn != nullptr)
			{
				if (!pawn->HasIFrames())
				{
					object->AddImpulse(GetVelocity().GetSafeNormal() * Impulse);
				}
			}
			else
			{
				object->AddImpulse(GetVelocity().GetSafeNormal() * Impulse);
			}
		}

		// Apply damage
		ICombatInterface* target = Cast<ICombatInterface>(OtherActor);
		if (target != nullptr)
		{
			target->Damage(Damage, DamageType, RumbleEffect, OtherComp, this, Source);
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
			FVector direction = (actors[i]->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// Apply physics
			IPhysicsInterface* object = Cast<IPhysicsInterface>(actors[i]);
			if (object != nullptr)
			{
				ACyberShooterPawn* pawn = Cast<ACyberShooterPawn>(OtherActor);
				if (pawn != nullptr)
				{
					if (!pawn->HasIFrames())
					{
						object->AddImpulse(GetVelocity().GetSafeNormal() * Impulse);
					}
				}
				else
				{
					object->AddImpulse(GetVelocity().GetSafeNormal() * Impulse);
				}
			}

			// Apply damage
			ICombatInterface* target = Cast<ICombatInterface>(actors[i]);
			if (target != nullptr)
			{
				target->Damage(Damage, DamageType, nullptr, OtherComp, this, Source);
			}
		}

		// Apply force feedback
		if (RumbleEffect != nullptr)
		{
			UGameplayStatics::SpawnForceFeedbackAtLocation(GetWorld(), RumbleEffect, GetActorLocation());
		}
	}
}