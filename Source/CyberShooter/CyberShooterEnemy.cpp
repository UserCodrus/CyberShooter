// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Components/CapsuleComponent.h"

ACyberShooterEnemy::ACyberShooterEnemy()
{
	// Apply the hit event handler
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterEnemy::OnHit);

	// Set defaults
	Momentum = MaxMomentum;
	DamageImmunity = DAMAGETYPE_ENEMY;

	AggroDistance = 5000.0f;

	DeathWeapon = nullptr;
	CollisionForce = 400.0f;
	ContactDamage = 1;
}

void ACyberShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ACyberShooterEnemy::Tick(float DeltaSeconds)
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
		FireShot(RootComponent->GetForwardVector(), RootComponent->GetUpVector());
	}
}

void ACyberShooterEnemy::Kill()
{
	// Switch to the death weapon and try to fire a shot
	ShotCooldown = 0.0f;
	Weapon = DeathWeapon;
	FireShot(RootComponent->GetForwardVector(), RootComponent->GetUpVector());

	Super::Kill();
}

void ACyberShooterEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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