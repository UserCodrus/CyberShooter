// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPawn.h"
#include "CyberShooterProjectile.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"
#include "Weapon.h"
#include "Ability.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Engine.h"

ACyberShooterPawn::ACyberShooterPawn()
{	
	// Create the collision component
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	CoreComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CoreComponent"));
	CoreComponent->SetupAttachment(RootComponent);

	// Set defaults
	GunOffset = 50.0f;
	FireWeapon = false;
	UseAbility = false;

	MaxHealth = 10;
	Health = 0;
	Armor = 0;
	Resistance = 0;
	MaxMomentum = 100.0f;
	Momentum = MaxMomentum;
	MomentumBonus = 0.0f;
	MomentumReward = 30.0f;
	MomentumBlockSize = 20.0f;
	MomentumPenalty = 1.0f;
	MomentumOverchargeDuration = 1.0f;
	TickSpeed = 1.0f;

	DamageDirection = FVector(1.0f, 0.0f, 0.0f);
	MinimumDamageAngle = -190.0f;
	MaximumDamageAngle = 190.0f;
	DamageImmunity = DAMAGETYPE_NONE;
	DamageCooldownDuration = 0.1f;
	EnvironmentDamage = 0.0f;
	BlinkRate = 20;

	ShotCooldown = 0.0f;
	AbilityCooldown = 0.0f;
	DamageCooldown = 0.0f;

	Ephemeral = true;
	ShowPawn = true;
}

void ACyberShooterPawn::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Momentum = MaxMomentum;

	// Call DisablePawn to ensure that the pawn is properly disabled
	if (Disabled)
	{
		Disabled = false;
		DisablePawn();
	}
}

void ACyberShooterPawn::Tick(float DeltaSeconds)
{
	// Set tick speed
	DeltaSeconds *= TickSpeed;

	// Reduce cooldowns
	if (ShotCooldown > 0.0f)
	{
		ShotCooldown -= DeltaSeconds * GetMomentumBonus();
	}
	if (AbilityCooldown > 0.0f)
	{
		AbilityCooldown -= DeltaSeconds;
	}

	// Blink after taking damage
	if (DamageCooldown > 0.0f)
	{
		DamageCooldown -= DeltaSeconds;
		if (ShowPawn)
		{
			int state = (int)(DamageCooldown * BlinkRate) & 2;
			SetActorHiddenInGame(!(bool)state);
		}
		else
		{
			SetActorHiddenInGame(!ShowPawn);
		}
	}
	else
	{
		SetActorHiddenInGame(!ShowPawn);
	}

	// Drain momentum when using abilities
	if (Ability != nullptr && UseAbility)
	{
		// Drain momentum from using an ability
		if (Ability->Continuous)
		{
			Momentum -= Ability->Cost * DeltaSeconds;
			if (Momentum <= 0.0f)
			{
				// Deactivate the ability when momentum is empty
				Momentum = 0.0f;
				StopAbility();
			}
		}
	}

	// Drain overcharged momentum
	if (Momentum > MaxMomentum)
	{
		Momentum -= MomentumBlockSize / MomentumOverchargeDuration * DeltaSeconds;

		// Never drain momentum below the max
		if (Momentum < MaxMomentum)
		{
			Momentum = MaxMomentum;
		}
	}
}

/// ICombatInterface ///

void ACyberShooterPawn::Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin)
{
	if (DamageCooldown > 0.0f)
		return;

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

		Value -= Resistance;
		if (Value > 0)
		{
			if (Armor > 0)
			{
				// Damage armor
				Armor -= Value;
				if (Armor < 0)
				{
					// Remove overflow damage from health
					Health += Armor;
					ChangeMomentum(-MomentumBlockSize * MomentumPenalty);
				}
			}
			else
			{
				// Damage health
				Health -= Value;
				ChangeMomentum(-MomentumBlockSize * MomentumPenalty);
			}
			DamageCooldown = DamageCooldownDuration;

			if (Health <= 0)
			{
				// Notify the player of kills
				ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(Origin);
				if (player != nullptr)
				{
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

	// Play force feedback
	if (DeathRumble != nullptr)
	{
		UGameplayStatics::SpawnForceFeedbackAtLocation(GetWorld(), DeathRumble, GetActorLocation());
	}

	if (Ephemeral)
	{
		Destroy();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Pawn destroyed");
	}
	else
	{
		DisablePawn();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Pawn disabled");
	}
}

/// Accessor Functions ///

void ACyberShooterPawn::StartFiring()
{
	FireWeapon = true;
}

void ACyberShooterPawn::StopFiring()
{
	FireWeapon = false;
}

void ACyberShooterPawn::DisablePawn()
{
	if (!Disabled)
	{
		Disabled = true;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);

		ShotCooldown = 0.0f;
		AbilityCooldown = 0.0f;
		DamageCooldown = 0.0f;
	}
}

void ACyberShooterPawn::EnablePawn()
{
	if (Disabled)
	{
		Disabled = false;
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}

void ACyberShooterPawn::StartAbility()
{
	if (Ability != nullptr && AbilityCooldown <= 0.0f)
	{
		if (!Ability->Continuous)
		{
			// Use the ability once
			if (Momentum >= Ability->Cost)
			{
				if (ActivateAbility())
				{
					// Drain momentum and set the cooldown timer
					Momentum -= Ability->Cost;
					AbilityCooldown = Ability->Cooldown;
				}
			}
		}
		else
		{
			// Activate the ability
			if (ActivateAbility())
			{
				UseAbility = true;
			}
		}
	}
}

void ACyberShooterPawn::StopAbility()
{
	if (UseAbility)
	{
		UseAbility = false;
		if (Ability != nullptr)
		{
			if (Ability->Continuous)
			{
				// Deactivate the ability and start the cooldown
				DeactivateAbility();
				AbilityCooldown = Ability->Cooldown;
			}
		}
	}
}

bool ACyberShooterPawn::ActivateAbility()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		// Retrieve the ability script then call it
		UAbilityScript* script = instance->GetScript(Ability->Script);
		if (script != nullptr)
		{
			return script->Activate(this);
		}
	}
	return false;
}

bool ACyberShooterPawn::DeactivateAbility()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		// Retrieve the ability script then call it
		UAbilityScript* script = instance->GetScript(Ability->Script);
		if (script != nullptr)
		{
			return script->Deactivate(this);
		}
	}
	return false;
}

float ACyberShooterPawn::GetMomentumBonus() const
{
	return 1.0f + Momentum / MomentumBlockSize * MomentumBonus;
}

void ACyberShooterPawn::StopAction()
{
	StopFiring();
	StopAbility();
}

void ACyberShooterPawn::Fire(FVector FireDirection)
{
	if (Weapon != nullptr && ShotCooldown <= 0.0f)
	{
		// If we are aiming in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			// Spawn a set of projectiles
			UWorld* world = GetWorld();
			if (world != nullptr)
			{
				FVector up = GetUpVector();
				float angle = -(Weapon->BulletOffset * (Weapon->NumBullets - 1)) / 2.0f;
				for (int32 i = 0; i < Weapon->NumBullets; ++i)
				{
					// Add a random variation to the shot angle based on weapon accuracy
					float random_angle = FMath::RandRange(-Weapon->FireAccuracy / 2.0f, Weapon->FireAccuracy / 2.0f);
					FRotator rotation = FireDirection.RotateAngleAxis(random_angle + angle, up).Rotation();

					// Spawn a projectile
					FVector location = GetActorLocation() + rotation.RotateVector(FVector(GunOffset, 0.0f, 0.0f));
					if (!Weapon->FanBullets)
					{
						rotation = FireDirection.RotateAngleAxis(random_angle, up).Rotation();
					}

					FActorSpawnParameters spawn_params;
					spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
					AActor* projectile = world->SpawnActor(Weapon->Projectile.Get(), &location, &rotation, spawn_params);
					if (projectile != nullptr)
					{
						Cast<ACyberShooterProjectile>(projectile)->SetSource(this);
					}

					// Increment angle
					angle += Weapon->BulletOffset;
				}
			}

			// Set the shot cooldown
			ShotCooldown = Weapon->FireRate;
		}
	}
}

FVector ACyberShooterPawn::GetForwardVector() const
{
	return RootComponent->GetForwardVector();
}

FVector ACyberShooterPawn::GetUpVector() const
{
	return RootComponent->GetUpVector();
}

FRotator ACyberShooterPawn::GetOrientationRotator() const
{
	return RootComponent->GetComponentRotation();
}

FVector ACyberShooterPawn::GetCoreForwardVector() const
{
	return CoreComponent->GetForwardVector();
}

FVector ACyberShooterPawn::GetCoreUpVector() const
{
	return CoreComponent->GetUpVector();
}

FRotator ACyberShooterPawn::GetCoreRotation() const
{
	return CoreComponent->GetComponentRotation();
}

FRotator ACyberShooterPawn::GetCoreLocalRotation() const
{
	return CoreComponent->GetRelativeRotation();
}

void ACyberShooterPawn::SetCoreRotation(FRotator Rotation)
{
	CoreComponent->SetRelativeRotation(Rotation);
}

void ACyberShooterPawn::SetCoreRotation(FQuat Rotation)
{
	CoreComponent->SetRelativeRotation(Rotation);
}

void ACyberShooterPawn::SetCoreWorldRotation(FRotator Rotation)
{
	CoreComponent->SetWorldRotation(Rotation);
}

void ACyberShooterPawn::SetCoreWorldRotation(FQuat Rotation)
{
	CoreComponent->SetWorldRotation(Rotation);
}

void ACyberShooterPawn::ChangeMomentum(float Value)
{
	if (Value < 0.0f && Momentum > MaxMomentum)
	{
		// Remove all overcharge
		Momentum = MaxMomentum;
	}
	else
	{
		// Add or remove momentum normally
		Momentum += Value;
	}

	// Cap momentum
	if (Momentum > MaxMomentum * 2)
	{
		Momentum = MaxMomentum * 2;
	}
	else if (Momentum < 0.0f)
	{
		Momentum = 0.0f;
	}
}

void ACyberShooterPawn::SetEnvironmentDamage(float Damage)
{
	EnvironmentDamage = Damage;
}

void ACyberShooterPawn::SetTickSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		TickSpeed = NewSpeed;
	}
}

void ACyberShooterPawn::AddArmor(int32 Value)
{
	Armor = Value;
}

void ACyberShooterPawn::AddHealth(int32 Value)
{
	Heal(Value);
}

void ACyberShooterPawn::AddMomentum(float Bars)
{
	ChangeMomentum(Bars * MomentumBlockSize);
}