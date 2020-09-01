// Copyright © 2020 Brian Faubion. All rights reserved.

#include "MovingPlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"

APlatformTrigger::APlatformTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a collision box to generate overlap events
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlatfromCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
}

AMovingPlatform::AMovingPlatform()
{
	// Enable ticking to apply velocity every frame
	PrimaryActorTick.bCanEverTick = true;

	// Adjust mesh settings for physics
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetGenerateOverlapEvents(true);
	mesh->OnComponentBeginOverlap.AddDynamic(this, &AMovingPlatform::BeginOverlap);

	mesh->SetMobility(EComponentMobility::Movable);
	mesh->SetSimulatePhysics(true);
	mesh->SetEnableGravity(false);
	mesh->bIgnoreRadialForce = true;
	mesh->bIgnoreRadialImpulse = true;
	mesh->BodyInstance.bLockXRotation = true;
	mesh->BodyInstance.bLockYRotation = true;
	mesh->BodyInstance.bLockZRotation = true;

	// Set movement defaults
	Direction = FVector(1.0f, 0.0f, 0.0f);
	Speed = 200.0f;
	Slow = 0.0f;
	Delay = 1.0f;
	DelayTimer = 0.0f;
}

void AMovingPlatform::Tick(float DeltaTime)
{
	// Set velocity if we aren't on a delay, otherwise use delay speed
	if (DelayTimer <= 0.0f)
	{
		GetStaticMeshComponent()->SetPhysicsLinearVelocity(Direction * Speed);
	}
	else
	{
		GetStaticMeshComponent()->SetPhysicsLinearVelocity(Direction * Slow);
		DelayTimer -= DeltaTime;
	}
}

void AMovingPlatform::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformTrigger* trigger = Cast<APlatformTrigger>(OtherActor);
	if (trigger != nullptr)
	{
		// Reverse the direction of the platform and set a delay timer when a trigger is hit
		Direction = -Direction;
		DelayTimer = Delay;
	}
}