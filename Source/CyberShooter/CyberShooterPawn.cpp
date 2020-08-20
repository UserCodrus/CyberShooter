// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPawn.h"
#include "CyberShooterProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName ACyberShooterPawn::MoveForwardBinding("MoveForward");
const FName ACyberShooterPawn::MoveRightBinding("MoveRight");
const FName ACyberShooterPawn::FireForwardBinding("FireForward");
const FName ACyberShooterPawn::FireRightBinding("FireRight");

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

	// Create the movement component
	//ShipMovementComponent = CreateDefaultSubobject<UPawnMovementComponent>(TEXT("ShipMovement"));
	//ShipMovementComponent->SetUpdatedComponent(RootComponent);
	//ShipMovementComponent->UpdatedComponent = RootComponent;

	// Set defaults
	MoveSpeed = 1000.0f;
	CollisionForce = 2000.0f;
	GunOffset = 90.f;
	FireWeapon = false;
	bCanFire = true;
}

void ACyberShooterPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		
		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}
	
	// Try and fire a shot
	if (FireWeapon && CurrentWeapon != nullptr)
	{
		// Calculate the direction to fire
		float FireForwardValue = GetInputAxisValue(FireForwardBinding);
		float FireRightValue = GetInputAxisValue(FireRightBinding);
		FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

		// Add random rotation based on weapon accuracy
		//FireDirection = FireDirection.RotateAngleAxis(FMath::RandRange(-CurrentWeapon->FireAccuracy / 2.0f, CurrentWeapon->FireAccuracy / 2.0f), FVector(0.0f, 0.0f, 1.0f));

		FireShot(FireDirection);
	}

}

void ACyberShooterPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACyberShooterPawn::StartFiring);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACyberShooterPawn::StopFiring);
}

void ACyberShooterPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
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
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
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
						world->SpawnActor(CurrentWeapon->Projectile.Get(), &location, &rotation);

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

			bCanFire = false;
			world->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ACyberShooterPawn::ShotTimerExpired, CurrentWeapon->FireRate / 2.0f);

			// try and play the sound if specified
			/*if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}*/
		}
	}
}

void ACyberShooterPawn::ShotTimerExpired()
{
	bCanFire = true;
}

