// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"
#include "EnemyTurretAI.h"
#include "SplineMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"

///
/// AEnemyBase ///
///

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
	Super::Tick(DeltaSeconds);

	// Fire the pawn's weapon
	if (FireWeapon)
	{
		Fire(CoreComponent->GetForwardVector());
	}
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

/// ICombatInterface ///

void AEnemyBase::Kill()
{
	// Switch to the death weapon and try to fire a shot
	ShotCooldown = 0.0f;
	Weapon = DeathWeapon;
	Fire(CoreComponent->GetForwardVector());

	Super::Kill();
}

///
/// AEnemyTurret ///
///

AEnemyTurret::AEnemyTurret()
{
	AIControllerClass = AEnemyTurretAI::StaticClass();

	ConstrainTargeting = false;
	RotationSpeed = 360.0f;
	MinimumFiringAngle = 180.0f;

	TrackPlayer = false;
}

void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();

	// Change ai settings
	AEnemyTurretAI* ai = Cast<AEnemyTurretAI>(GetController());
	if (ai != nullptr)
	{
		ai->SmartAim = TrackPlayer;
	}
}

///
/// AEnemyMovingTurret ///
///

AEnemyMovingTurret::AEnemyMovingTurret()
{
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetAbsolute(true, false, true);
	Spline->Duration = 10.0f;

	MovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	MovementComponent->Spline = Spline;
}

FVector AEnemyMovingTurret::GetVelocity() const
{
	return MovementComponent->Velocity;
}

/// Spline Utilities ///

void AEnemyMovingTurret::MoveEnemyToStart()
{
	SetActorLocation(Spline->GetComponentLocation());
}

void AEnemyMovingTurret::MoveEnemyToEnd()
{
	SetActorLocation(Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1));
}

void AEnemyMovingTurret::ResetSpline()
{
	// Remove extra points
	for (int32 i = Spline->GetNumberOfSplinePoints() - 1; i > 0; --i)
	{
		Spline->RemoveSplinePoint(i);
	}

	// Reset the location of the spline
	Spline->SetWorldLocation(GetActorLocation());
}

void AEnemyMovingTurret::MoveSpline()
{
	Spline->SetWorldLocation(GetActorLocation());
}

void AEnemyMovingTurret::AppendSpline()
{
	Spline->AddSplineWorldPoint(GetActorLocation());
}