// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayer.h"
#include "PlayerMovementComponent.h"
#include "CyberShooterPlayerController.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	SpringArmComponent->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerTopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	MovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	Momentum = 0.0f;
	MomentumBonus = 0.5f;
	MomentumReward = 0.0f;
	MomentumPenalty = -20.0f;

	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);

	CameraVector = FVector(0.1f, 0.0f, -1.0f);
}

/// APawn Functions ///

void ACyberShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->ChangeCollision(CollisionComponent);
}

void ACyberShooterPlayer::Tick(float DeltaSeconds)
{
	// Get the current rotation of the pawn relative to the up and forward vectors
	FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);

	// Rotate the camera to match the world rotation
	SpringArmComponent->SetRelativeRotation(world_rotation.RotateVector(CameraVector.GetUnsafeNormal()).Rotation());
	
	// Find movement direction
	FVector move_direction = FVector(GetInputAxisValue(MoveForwardBinding), GetInputAxisValue(MoveRightBinding), 0.0f).GetClampedToMaxSize(1.0f);
	
	// Move the pawn
	MovementComponent->ChangeSpeed(1.0f + (Momentum / MaxMomentum) * MomentumBonus);
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
		if (FireWeapon)
		{
			// Fire in the direction the player is aiming or in the direction the pawn is facing
			FVector fire_direction = world_rotation.RotateVector(controller->GetFireDirection());
			if (fire_direction.IsNearlyZero())
			{
				fire_direction = GetActorRotation().Vector();
			}
			FireShot(fire_direction);
		}
	}
}

void ACyberShooterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACyberShooterPawn::StartFiring);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACyberShooterPawn::StopFiring);
}

/// IBreakable Functions ///

void ACyberShooterPlayer::Kill()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Player has been killed");
}

/// Accessor functions ///

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

void ACyberShooterPlayer::StopFiring()
{
	FireWeapon = false;
}