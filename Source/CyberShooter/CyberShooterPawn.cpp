// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPawn.h"
#include "CyberShooterProjectile.h"
#include "CyberShooterPlayer.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"

ACyberShooterPawn::ACyberShooterPawn()
{	
	// Create the collision component
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerCollisionComponent"));
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterPawn::OnHit);
	RootComponent = CollisionComponent;

	// Create the mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMeshComponent"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->SetupAttachment(RootComponent);

	// Set defaults
	CollisionForce = 2000.0f;
	GunOffset = 90.0f;
	FireWeapon = false;

	MaxHealth = 10;
	Health = 0;
	MaxMomentum = 100.0f;
	Momentum = MaxMomentum;
	MomentumBonus = 0.0f;
	MomentumReward = 30.0f;
	MomentumPenalty = 0.0f;

	DamageDirection = FVector(1.0f, 0.0f, 0.0f);
	MinimumDamageAngle = -190.0f;
	MaximumDamageAngle = 190.0f;
	DamageImmunity = 0;

	CanFire = true;
}

void ACyberShooterPawn::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

void ACyberShooterPawn::Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin)
{
	if (!(DamageImmunity & DamageType))
	{
		if (Source != nullptr)
		{
			// Calculate the angle of the incoming damage on the Z axis
			FVector source_direction = GetActorLocation() - Source->GetActorLocation();
			FVector damage_direction = GetActorRotation().RotateVector(DamageDirection);
			float angle = (source_direction.Rotation() - damage_direction.Rotation()).Clamp().Yaw - 180.0f;

			// Determine if the hit failed
			if (angle < MinimumDamageAngle || angle > MaximumDamageAngle)
			{
				return;
			}
		}
		
		if (Value > 0)
		{
			Health -= Value;
			ChangeMomentum(MomentumPenalty);

			if (Health <= 0.0f)
			{
				// Notify the player of kills
				ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(Origin);
				if (player != nullptr)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Enemy killed");
					player->ChangeMomentum(MomentumReward);
				}

				Kill();
			}
		}
	}
}

void ACyberShooterPawn::Heal(int32 Value)
{
	if (Value > 0)
	{
		Health += Value;

		if (Health > MaxHealth)
		{
			Health = MaxHealth;
		}
	}
}

void ACyberShooterPawn::Kill()
{
	// Create death particles
	if (DeathParticles != nullptr)
	{
		FTransform transform;
		transform.SetLocation(GetActorLocation());
		transform.SetRotation(GetActorRotation().Quaternion());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticles, transform);
	}

	// Play the death sound
	if (DeathSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	Destroy();
}

void ACyberShooterPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Add an impulse when a pysics object is hit
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetActorRotation().Vector() * CollisionForce, GetActorLocation());
	}
}

void ACyberShooterPawn::StartFiring()
{
	FireWeapon = true;
}

void ACyberShooterPawn::StopFiring()
{
	FireWeapon = false;
}

void ACyberShooterPawn::FireShot(FVector FireDirection)
{
	if (CanFire == true)
	{
		// If we are aiming in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			// Spawn a set of projectiles
			UWorld* world = GetWorld();
			if (world != nullptr)
			{
				if (CurrentWeapon->NumBullets > 1)
				{
					// Spawn multiple bullets in a fan pattern
					float angle = -(CurrentWeapon->BulletOffset * (CurrentWeapon->NumBullets - 1)) / 2.0f;
					for (int32 i = 0; i < CurrentWeapon->NumBullets; ++i)
					{
						// Add a random variation to the shot angle based on weapon accuracy
						float random_angle = FMath::RandRange(-CurrentWeapon->FireAccuracy / 2.0f, CurrentWeapon->FireAccuracy / 2.0f);
						FRotator rotation = FireDirection.RotateAngleAxis(random_angle + angle, FVector(0.0f, 0.0f, 1.0f)).Rotation();

						// Spawn a projectile
						FVector location = GetActorLocation() + rotation.RotateVector(FVector(GunOffset, 0.0f, 0.0f));
						if (!CurrentWeapon->FanBullets)
						{
							rotation = FireDirection.RotateAngleAxis(random_angle, FVector(0.0f, 0.0f, 1.0f)).Rotation();
						}

						AActor* projectile = world->SpawnActor(CurrentWeapon->Projectile.Get(), &location, &rotation);
						if (projectile != nullptr)
						{
							Cast<ACyberShooterProjectile>(projectile)->SetSource(this);
						}

						// Increment angle
						angle += CurrentWeapon->BulletOffset;
					}
				}
				else
				{
					// Spawn a single bullet
					FRotator rotation = FireDirection.RotateAngleAxis(FMath::RandRange(-CurrentWeapon->FireAccuracy / 2.0f, CurrentWeapon->FireAccuracy / 2.0f), FVector(0.0f, 0.0f, 1.0f)).Rotation();
					FVector location = GetActorLocation() + rotation.RotateVector(FVector(GunOffset, 0.0f, 0.0f));
					world->SpawnActor(CurrentWeapon->Projectile.Get(), &location, &rotation);
				}
			}

			CanFire = false;
			world->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ACyberShooterPawn::ShotTimerExpired, CurrentWeapon->FireRate / 2.0f);
		}
	}
}

void ACyberShooterPawn::ShotTimerExpired()
{
	CanFire = true;
}

void ACyberShooterPawn::ChangeMomentum(float Value)
{
	Momentum += Value;

	if (Momentum > MaxMomentum)
	{
		Momentum = MaxMomentum;
	}
	else if (Momentum < 0.0f)
	{
		Momentum = 0.0f;
	}
}