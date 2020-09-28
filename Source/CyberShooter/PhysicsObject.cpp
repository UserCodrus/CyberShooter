// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsObject.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterGameInstance.h"

APhysicsObject::APhysicsObject()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("PhysicsMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Make the mesh movable
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
}

void APhysicsObject::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->SetOrientation(GetActorForwardVector(), GetActorUpVector());

	ResetGravity();
	ResetAirFriction();
}

/// IBreakable Interface ///

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

void APhysicsObject::SetGravityEnabled(bool Enable)
{
	MovementComponent->GravityEnabled = Enable;
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

void APhysicsObject::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		MovementComponent->SetTickSpeed(NewRate);
	}
}