// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"
#include "EnemyTurretAI.h"
#include "EnemySeekerAI.h"
#include "SplineMovementComponent.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterGameInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"

///
/// AEnemyBase ///
///

AEnemyBase::AEnemyBase()
{
	// Create the aiming component
	AimComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimComponent"));
	AimComponent->SetupAttachment(RootComponent);

	CollisionComponent->SetCollisionProfileName("Enemy");

	// Apply the hit event handler
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemyBase::OnHit);

	// Set defaults
	Momentum = MaxMomentum;
	DamageImmunity = DAMAGETYPE_ENEMY;

	DeathWeapon = nullptr;
	CollisionForce = 1000.0f;
	ContactDamage = 1;

	AggroLevel = 0;
	RequiredAggro = 1;
	MinimumAggro = 0;
	AIDistance = 500.0f;
	RespawnDuration = 5.0f;
	RespawnCooldown = 2.0f;
	BlinkRate = 20;

	RespawnTimer = 0.0f;
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
		Fire(AimComponent->GetForwardVector());
	}

	// Manage the respawn timer
	if (RespawnTimer > 0.0f)
	{
		RespawnTimer -= DeltaSeconds;
		if (RespawnTimer <= 0.0f)
		{
			if (Respawned)
			{
				// Check for collisions at the enemy's position and respawn if the location is clear, otherwise restart the spawn cooldown
				FHitResult hit;
				if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), GetActorQuat(), CollisionComponent->GetCollisionObjectType(), CollisionComponent->GetCollisionShape()))
				{
					RespawnTimer = RespawnCooldown;
				}
				else
				{
					CancelRespawn();
				}
			}
			else
			{
				// Respawn the enemy
				Respawn();
			}
		}
		else
		{
			// Make the enemy blink
			int state = (int)(RespawnTimer * BlinkRate) & 2;
			SetActorHiddenInGame((bool)state);
		}
	}
}

void AEnemyBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Apply contact damage and knockback to players
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		FVector direction = GetOrientationRotator().UnrotateVector(player->GetActorLocation() - GetActorLocation());
		direction.Z = 0.0f;
		direction = GetOrientationRotator().RotateVector(direction).GetSafeNormal();

		player->AddImpulse(direction * CollisionForce);

		if (!player->HasIFrames())
		{
			player->Damage(ContactDamage, DAMAGETYPE_ENEMY, this, this);
		}
	}
}

void AEnemyBase::StartRespawn()
{
	RespawnTimer = RespawnDuration;
}

void AEnemyBase::CancelRespawn()
{
	// Clear the respawn timer and any respawn effects
	RespawnTimer = 0.0f;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Respawned = false;

	// Activate AI if needed
	if (AggroLevel >= RequiredAggro)
	{
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->Active = true;
			OnAggro();
		}
	}
}

void AEnemyBase::Respawn()
{
	// Disable collision and set the respawn timer to prevent the enemy from popping in on top of the player
	SetActorEnableCollision(false);
	Respawned = true;
	RespawnTimer = RespawnCooldown;

	// Deactivate AI
	AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
	if (controller != nullptr)
	{
		controller->Active = false;
		EndAggro();
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

/// Accessors ///

FVector AEnemyBase::GetAimVector() const
{
	return AimComponent->GetForwardVector();
}

FRotator AEnemyBase::GetAimRotation() const
{
	return AimComponent->GetComponentRotation();
}

FRotator AEnemyBase::GetAimLocalRotation() const
{
	return AimComponent->GetRelativeRotation();
}

void AEnemyBase::SetAimRotation(FRotator Rotation)
{
	AimComponent->SetRelativeRotation(Rotation);
}

void AEnemyBase::SetAimRotation(FQuat Rotation)
{
	AimComponent->SetRelativeRotation(Rotation);
}

void AEnemyBase::SetAimWorldRotation(FRotator Rotation)
{
	AimComponent->SetWorldRotation(Rotation);
}

void AEnemyBase::SetAimWorldRotation(FQuat Rotation)
{
	AimComponent->SetWorldRotation(Rotation);
}

void AEnemyBase::Aggro()
{
	AggroLevel++;

	if (AggroLevel >= RequiredAggro)
	{
		// Activate AI
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->Active = true;
			OnAggro();
		}
	}
}

void AEnemyBase::EndAggro()
{
	AggroLevel--;

	if (AggroLevel <= MinimumAggro)
	{
		// Deactivate AI
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->Active = false;
			OnEndAggro();
		}
	}
}

bool AEnemyBase::IsAggro()
{
	return AggroLevel > 0;
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
}

void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();

	// Change ai settings
	AEnemyTurretAI* ai = Cast<AEnemyTurretAI>(GetController());
	if (ai != nullptr)
	{
		//ai->SmartAim = TrackPlayer;
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

///
/// AEnemySeeker ///
///

AEnemySeeker::AEnemySeeker()
{
	AIControllerClass = AEnemySeekerAI::StaticClass();

	MovementComponent = CreateDefaultSubobject<UEnemyMovementComponent>(TEXT("EnemyMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Set defaults
	GravityMultiplier = 1.0f;
	WorldGravity = 0.0f;
	Mass = 1.0f;
}

void AEnemySeeker::BeginPlay()
{
	Super::BeginPlay();

	// Set up the movement component
	MovementComponent->Height = CollisionComponent->GetScaledCapsuleHalfHeight();

	// Set world physics
	ResetGravity();
	ResetAirFriction();
	ResetMass();

	RespawnLocation = GetActorLocation();

	AEnemySeekerAI* ai = Cast<AEnemySeekerAI>(GetController());
	if (ai != nullptr)
	{
		//ai->MovementOffset = 180.0f;
	}
}

FVector AEnemySeeker::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void AEnemySeeker::Respawn()
{
	AEnemyBase::Respawn();
	MovementComponent->Teleport(RespawnLocation);
}

void AEnemySeeker::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			// Handle the collision
			MovementComponent->Impact(object, Hit.Normal);
		}

		// Apply contact damage and knockback to players
		ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
		if (player != nullptr)
		{
			FVector direction = player->GetActorLocation() - GetActorLocation();
			player->AddImpulse(direction.GetSafeNormal() * CollisionForce);

			if (!player->HasIFrames())
			{
				player->Damage(ContactDamage, DAMAGETYPE_ENEMY, this, this);
			}
		}
	}
}

void AEnemySeeker::AddControlInput(FVector Direction)
{
	MovementComponent->AddControlInput(Direction);
}

/// IPhysicsInterface ///

FVector AEnemySeeker::GetVelocity()
{
	return MovementComponent->GetTotalVelocity();
}

void AEnemySeeker::ChangeVelocity(FVector NewVelocity)
{
	MovementComponent->SetVelocity(NewVelocity);
}

void AEnemySeeker::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void AEnemySeeker::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(CoreComponent->GetComponentRotation().RotateVector(Force));
}

void AEnemySeeker::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void AEnemySeeker::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void AEnemySeeker::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void AEnemySeeker::SetGravity(float NewGravity)
{
	WorldGravity = NewGravity;
	MovementComponent->Gravity = GravityMultiplier * WorldGravity;
}

void AEnemySeeker::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		WorldGravity = instance->GetGravity();
		MovementComponent->Gravity = GravityMultiplier * WorldGravity;
	}
}

void AEnemySeeker::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void AEnemySeeker::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void AEnemySeeker::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void AEnemySeeker::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void AEnemySeeker::SetMass(float Multiplier)
{
	MovementComponent->Mass = Mass * Multiplier;
}

void AEnemySeeker::ResetMass()
{
	MovementComponent->Mass = Mass;
}

void AEnemySeeker::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		TickSpeed = NewRate;
		MovementComponent->SetTickSpeed(NewRate);
	}
}

float AEnemySeeker::GetMass() const
{
	return MovementComponent->Mass;
}

float AEnemySeeker::GetWeight() const
{
	return MovementComponent->GetGravity() * Mass;
}

/// Blueprint interface ///

void AEnemySeeker::BPAddImpulse(FVector Force)
{
	AddImpulse(Force);
}

void AEnemySeeker::BPAddRelativeImpulse(FVector Force)
{
	AddRelativeImpulse(Force);
}