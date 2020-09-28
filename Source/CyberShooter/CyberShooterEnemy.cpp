// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"
#include "EnemyTurretAI.h"

#include "Components/CapsuleComponent.h"

AEnemyBase::AEnemyBase()
{
	// Apply the hit event handler
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemyBase::OnHit);

	// Set defaults
	Momentum = MaxMomentum;
	DamageImmunity = DAMAGETYPE_ENEMY;

	AggroDistance = 5000.0f;

	DeathWeapon = nullptr;
	CollisionForce = 400.0f;
	ContactDamage = 1;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	// Set tick speed
	DeltaSeconds *= TickSpeed;
	
	// Reduce cooldowns
	if (ShotCooldown > 0.0f)
	{
		ShotCooldown -= DeltaSeconds;
	}
	if (AbilityCooldown > 0.0f)
	{
		AbilityCooldown -= DeltaSeconds;
	}

	// Drain momentum when using abilities
	SustainAbility(DeltaSeconds);

	// Fire the pawn's weapon
	if (FireWeapon)
	{
		Fire(CoreComponent->GetForwardVector());
	}
}

void AEnemyBase::Kill()
{
	// Switch to the death weapon and try to fire a shot
	ShotCooldown = 0.0f;
	Weapon = DeathWeapon;
	Fire(CoreComponent->GetForwardVector());

	Super::Kill();
}

void AEnemyBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Apply contact damage and knockback to players
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		FVector direction = player->GetActorLocation() - GetActorLocation();
		player->StopMovement();
		player->AddImpulse(direction.GetSafeNormal() * CollisionForce);

		player->Damage(ContactDamage, DAMAGETYPE_ENEMY, this, this);
	}
}

/// AEnemyTurret ///

AEnemyTurret::AEnemyTurret()
{
	AIControllerClass = AEnemyTurretAI::StaticClass();

	ConstrainTargeting = false;
	RotationSpeed = 360.0f;
	MinimumFiringAngle = 180.0f;
}