// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayer.h"
#include "PlayerMovementComponent.h"
#include "CyberShooterPlayerController.h"
#include "CyberShooterGameInstance.h"
#include "TeleportTrigger.h"
#include "Weapon.h"
#include "Ability.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Engine.h"

const FName ACyberShooterPlayer::MoveForwardBinding("MoveForward");
const FName ACyberShooterPlayer::MoveRightBinding("MoveRight");

ACyberShooterPlayer::ACyberShooterPlayer()
{
	// Set up a fixed camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerSpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = 1200.f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerTopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetUsingAbsoluteRotation(true);

	MovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	RespawnMarker = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnMarker"));
	RespawnMarker->SetupAttachment(RootComponent);
	RespawnMarker->SetAbsolute(true, true, true);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterPlayer::OnHit);

	// Set properties
	Momentum = 0.0f;
	MomentumBonus = 0.5f;
	MomentumReward = 0.0f;
	MomentumPenalty = -20.0f;

	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);
	PreviousForward = Forward;
	PreviousUp = Up;
	SpeedMultiplier = 1.0f;
	GravityMultiplier = 1.0f;
	RespawnDistance = 10000.0f;
	DamageImmunity = DAMAGETYPE_PLAYER;

	CameraAngle = FRotator(-75.0f, 0.0f, 0.0f);
	CameraTransitionTime = 1.0f;

	// Set other values
	LastTeleportTarget = nullptr;
	CameraStartRotation = FQuat();
	CameraDesiredRotation = FQuat();
	CameraTimer = 0.0f;
	FloorDamageTimer = 0.0f;
}

/// APawn Functions ///

void ACyberShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Set up the movement component
	MovementComponent->SetCollision(CollisionComponent);
	if (!SetOrientation(Forward, Up, true))
	{
		// Reset forward and up vectors if invalid vectors are set
		Forward = FVector(1.0f, 0.0f, 0.0f);
		Up = FVector(0.0f, 0.0f, 1.0f);

		MovementComponent->SetOrientation(Forward, Up);
	}

	// Set world physics
	ResetWorldGravity();
	ResetAirFriction();
}

void ACyberShooterPlayer::Tick(float DeltaSeconds)
{
	// Set tick speed
	DeltaSeconds *= TickSpeed;

	// Get the current rotation of the pawn relative to the up and forward vectors
	FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);

	// Handle camera transitions
	if (CameraTimer > 0.0f)
	{
		CameraTransition(DeltaSeconds);
	}

	// Reduce cooldowns
	if (ShotCooldown > 0.0f)
	{
		ShotCooldown -= DeltaSeconds;
	}
	if (AbilityCooldown > 0.0f)
	{
		AbilityCooldown -= DeltaSeconds;
	}
	
	// Drain momentum if the player is using an ability
	SustainAbility(DeltaSeconds);
	
	// Adjust movement attributes
	MovementComponent->SetSpeed((1.0f + (Momentum / MaxMomentum) * MomentumBonus) * SpeedMultiplier);

	// Apply movement inputs
	FVector move_direction = FVector(GetInputAxisValue(MoveForwardBinding), GetInputAxisValue(MoveRightBinding), 0.0f).GetClampedToMaxSize(1.0f);
	AddMovementInput(move_direction);

	// Rotate to face the movement direction
	if (move_direction.SizeSquared() > 0.0f)
	{
		RootComponent->SetWorldRotation(world_rotation.RotateVector(move_direction).Rotation());
	}

	// Try to fire a shot if the radial menus aren't open and the player is shooting
	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr && !controller->IsMenuOpen() && FireWeapon)
	{
		// Fire in the direction the player is aiming or in the direction the pawn is facing
		FVector fire_direction = world_rotation.RotateVector(controller->GetFireDirection());
		if (fire_direction.IsNearlyZero())
		{
			fire_direction = GetActorRotation().Vector();
		}
		FireShot(fire_direction, Up);
	}

	// Take damage from environmental hazards
	if (MovementComponent->GetFloorDamage() > 0.0f || EnvironmentDamage > 0.0f)
	{
		FloorDamageTimer += (MovementComponent->GetFloorDamage() + EnvironmentDamage) * DeltaSeconds;

		if (FloorDamageTimer >= 1.0f)
		{
			int32 damage = FMath::TruncToInt(FloorDamageTimer);
			Damage(damage, DAMAGETYPE_ENVIRONMENT, nullptr, nullptr);
			FloorDamageTimer -= (float)damage;
		}
	}
	else
	{
		FloorDamageTimer = 0.0f;
	}

	// Respawn when out of bounds
	if (IsFalling())
	{
		if (FVector::DistSquared(GetActorLocation(), RespawnMarker->GetComponentLocation()) > RespawnDistance * RespawnDistance)
		{
			Respawn();
		}
	}
	else
	{
		if (IsStable())
		{
			RespawnPoint = GetActorLocation();
			RespawnMarker->SetWorldLocation(RespawnPoint);
		}
	}
}

void ACyberShooterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACyberShooterPlayer::StartFiring);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACyberShooterPlayer::StopFiring);
	PlayerInputComponent->BindAction("Ability", EInputEvent::IE_Pressed, this, &ACyberShooterPlayer::StartAbility);
	PlayerInputComponent->BindAction("Ability", EInputEvent::IE_Released, this, &ACyberShooterPlayer::StopAbility);
}

FVector ACyberShooterPlayer::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void ACyberShooterPlayer::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		// Apply physics impulses
		if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * CollisionForce, GetActorLocation());
		}
	}
}

/// IBreakable Functions ///

void ACyberShooterPlayer::Kill()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Player has been killed");
}

/// Accessor functions ///

bool ACyberShooterPlayer::IsFalling() const
{
	return MovementComponent->IsFalling();
}

bool ACyberShooterPlayer::IsStable() const
{
	return MovementComponent->IsStable();
}

void ACyberShooterPlayer::SelectWeapon(int32 Slot)
{
	if (Slot >= 0 && Slot < WeaponSet.Num())
	{
		Weapon = WeaponSet[Slot];
	}
}

void ACyberShooterPlayer::AddWeapon(UWeapon* NewWeapon)
{
	// Avoid adding duplicate weapons
	for (int32 i = 0; i < WeaponSet.Num(); ++i)
	{
		if (WeaponSet[i] == NewWeapon)
		{
			return;
		}
	}

	WeaponSet.Add(NewWeapon);
}

void ACyberShooterPlayer::SelectAbility(int32 Slot)
{
	if (Slot >= 0 && Slot < AbilitySet.Num())
	{
		Ability = AbilitySet[Slot];
	}
}

void ACyberShooterPlayer::AddAbility(UAbility* NewAbility)
{
	// Avoid adding duplicate weapons
	for (int32 i = 0; i < AbilitySet.Num(); ++i)
	{
		if (AbilitySet[i] == NewAbility)
		{
			return;
		}
	}

	AbilitySet.Add(NewAbility);
}

bool ACyberShooterPlayer::SetOrientation(FVector NewForward, FVector NewUp, bool SnapCamera)
{
	// Normalize the vectors
	NewForward = NewForward.GetSafeNormal();
	NewUp = NewUp.GetSafeNormal();

	// Make sure the vectors are orthogonal
	if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
	{
		if (FVector::Orthogonal(NewForward, NewUp))
		{
			PreviousForward = Forward;
			PreviousUp = Up;

			Forward = NewForward;
			Up = NewUp;

			// Set the movement component to match our orientation
			MovementComponent->SetOrientation(Forward, Up);

			// Change the pawn's rotation to match the new orientation
			FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);
			RootComponent->SetWorldRotation(world_rotation.RotateVector(FVector(1.0f, 0.0f, 0.0f)).Rotation());

			if (SnapCamera)
			{
				// Rotate the camera to match the new orientation
				SpringArmComponent->SetRelativeRotation(world_rotation);
				SpringArmComponent->AddLocalRotation(CameraAngle);
				CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());
			}
			else
			{
				// Set up the camera transition
				CameraStartRotation = SpringArmComponent->GetRelativeRotation().Quaternion();
				SpringArmComponent->SetRelativeRotation(world_rotation);
				SpringArmComponent->AddLocalRotation(CameraAngle);
				CameraDesiredRotation = SpringArmComponent->GetRelativeRotation().Quaternion();

				// Go back to the original rotation and start the transition
				SpringArmComponent->SetRelativeRotation(CameraStartRotation);
				ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
				if (controller != nullptr)
				{
					controller->SetCameraTransition(true);
					CameraTimer = CameraTransitionTime * controller->GetTransitionSpeed();
				}
				else
				{
					CameraTimer = CameraTransitionTime;
				}
			}
			

			return true;
		}
	}

	return false;
}

bool ACyberShooterPlayer::RevertOrientation(bool SnapCamera)
{
	return SetOrientation(PreviousForward, PreviousUp, SnapCamera);
}

bool ACyberShooterPlayer::CheckOrientation(FVector RequiredUp) const
{
	return Up.GetSafeNormal().Equals(RequiredUp.GetSafeNormal());
}

void ACyberShooterPlayer::SetSpeedMultiplier(float NewValue)
{
	if (NewValue > 0.0f)
	{
		SpeedMultiplier = NewValue;
	}
}

void ACyberShooterPlayer::SetGravityMultiplier(float NewValue)
{
	GravityMultiplier = NewValue;
	MovementComponent->SetGravity(GravityMultiplier *WorldGravity);
}

void ACyberShooterPlayer::SetWorldGravity(float NewValue)
{
	WorldGravity = NewValue;
	MovementComponent->SetGravity(GravityMultiplier * WorldGravity);
}

void ACyberShooterPlayer::ResetWorldGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		WorldGravity = instance->GetGravity();
		MovementComponent->SetGravity(GravityMultiplier * WorldGravity);
	}
}

void ACyberShooterPlayer::SetGravityEnabled(bool Enable)
{
	MovementComponent->EnableGravity(Enable);
}

void ACyberShooterPlayer::SetAirFriction(float NewFriction)
{
	MovementComponent->SetAirFriction(NewFriction);
}

void ACyberShooterPlayer::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->SetAirFriction(instance->GetAirFriction());
	}
}

void ACyberShooterPlayer::SetFriction(float NewFriction)
{
	MovementComponent->SetFriction(NewFriction);
}

void ACyberShooterPlayer::ResetFriction()
{
	MovementComponent->SetFriction(1.0f);
}

void ACyberShooterPlayer::AddImpulse(FVector Impulse)
{
	if (!Impulse.IsNearlyZero())
	{
		MovementComponent->AddImpulse(Impulse);
	}
}

void ACyberShooterPlayer::AddRelativeImpulse(FVector Impulse)
{
	if (!Impulse.IsNearlyZero())
	{
		// Rotate the imput vector to make it relative to the player's frame of reference
		FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);
		MovementComponent->AddImpulse(world_rotation.RotateVector(Impulse));
	}
}

void ACyberShooterPlayer::SetStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void ACyberShooterPlayer::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void ACyberShooterPlayer::StopGravity()
{
	MovementComponent->StopGravity();
}

void ACyberShooterPlayer::StopMovement()
{
	MovementComponent->StopMovement();
}

void ACyberShooterPlayer::SetTickSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		TickSpeed = NewSpeed;
		MovementComponent->SetTickSpeed(NewSpeed);
	}
}

void ACyberShooterPlayer::Teleport(ATeleportBase* Teleporter, ATeleportBase* Target)
{
	if (Teleporter != LastTeleportTarget)
	{
		LastTeleportTarget = Target;

		// Teleport to the designated teleporter
		TeleportTo(Target->GetActorLocation(), GetActorRotation());
		Target->OrientPlayer(this);
	}
}

bool ACyberShooterPlayer::CheckTeleport(ATeleportBase* Target)
{
	if (Target == LastTeleportTarget)
	{
		// Reset the teleport target
		LastTeleportTarget = nullptr;

		return false;
	}
	return true;
}

void ACyberShooterPlayer::CameraTransition(float DeltaTime)
{
	CameraTimer -= DeltaTime;
	if (CameraTimer > 0.0f)
	{
		float dilation = 1.0f;
		ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
		if (controller != nullptr)
		{
			dilation = controller->GetTransitionSpeed();
		}

		// Interpolate the camera rotation
		SpringArmComponent->SetRelativeRotation(FQuat::Slerp(CameraDesiredRotation, CameraStartRotation, CameraTimer / (CameraTransitionTime * dilation)));
		CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());
	}
	else
	{
		// Set the final rotation
		SpringArmComponent->SetRelativeRotation(CameraDesiredRotation);
		CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());

		// Stop the transition
		ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
		if (controller != nullptr)
		{
			controller->SetCameraTransition(false);
		}
	}
}

void ACyberShooterPlayer::Respawn()
{
	TeleportTo(RespawnMarker->GetComponentLocation(), GetActorRotation());
	Damage(1, DAMAGETYPE_ENVIRONMENT);
}