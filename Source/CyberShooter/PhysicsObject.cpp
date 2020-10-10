// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsObject.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/KismetMathLibrary.h"

APhysicsObject::APhysicsObject()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("PhysicsMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Make the mesh movable
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APhysicsObject::OnHit);

	CanChangeOrientation = false;
	RespawnDistance = 10000.0f;
	Mass = 1.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void APhysicsObject::BeginPlay()
{
	Super::BeginPlay();

	// Figure out how far to do floor checks
	FVector min, max;
	GetStaticMeshComponent()->GetLocalBounds(min, max);
	MovementComponent->Height = FMath::Abs(min.Z) * GetActorScale().Z;

	// Apply the current orientation to the movement component
	InitialForward = GetActorForwardVector();
	InitialUp = GetActorUpVector();
	MovementComponent->SetOrientation(GetActorForwardVector(), GetActorUpVector());

	RespawnPoint = GetActorLocation();

	// Set world physics
	ResetGravity();
	ResetAirFriction();
	ResetMass();
}

void APhysicsObject::Tick(float DeltaSeconds)
{
	// Manage respawning
	if (FVector::DistSquared(GetActorLocation(), RespawnPoint) > RespawnDistance * RespawnDistance)
	{
		MovementComponent->Teleport(RespawnPoint);
		SetOrientation(InitialForward, InitialUp);
	}
}

FVector APhysicsObject::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void APhysicsObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetVelocity().IsNearlyZero())
		return;

	if (OtherActor != nullptr && OtherActor != this)
	{
		// Apply physics impulses
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			float alignment = FMath::Clamp(-FVector::DotProduct(GetVelocity().GetSafeNormal(), Hit.ImpactNormal), 0.0f, 1.0f);
			object->AddImpulse(MovementComponent->GetForce() * alignment);
			MovementComponent->Velocity *= 0.5f;
		}
	}
}

/// IPhysicsInterface ///

void APhysicsObject::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void APhysicsObject::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(RootComponent->GetComponentRotation().RotateVector(Force));
}

void APhysicsObject::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void APhysicsObject::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void APhysicsObject::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void APhysicsObject::SetGravity(float NewGravity)
{
	MovementComponent->Gravity = NewGravity;
}

void APhysicsObject::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->Gravity = instance->GetGravity();
	}
}

void APhysicsObject::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void APhysicsObject::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void APhysicsObject::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void APhysicsObject::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void APhysicsObject::SetMass(float Multiplier)
{
	MovementComponent->Mass = Mass * Multiplier;
}

void APhysicsObject::ResetMass()
{
	MovementComponent->Mass = Mass;
}

void APhysicsObject::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		MovementComponent->SetTickSpeed(NewRate);
	}
}

float APhysicsObject::GetWeight() const
{
	return MovementComponent->GetGravity() * MovementComponent->Mass;
}

/// IOrientationInterface ///

bool APhysicsObject::SetOrientation(FVector NewForward, FVector NewUp)
{
	if (CanChangeOrientation)
	{
		if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
		{
			if (FVector::Orthogonal(NewForward, NewUp))
			{
				FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp);
				RootComponent->SetWorldRotation(world_rotation);

				MovementComponent->SetOrientation(GetActorForwardVector(), GetActorUpVector());

				return true;
			}
		}
	}

	return false;
}

bool APhysicsObject::CheckOrientation(FVector DesiredUp)
{
	return GetActorUpVector().Equals(DesiredUp.GetSafeNormal());
}