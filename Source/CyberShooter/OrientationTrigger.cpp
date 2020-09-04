// Copyright © 2020 Brian Faubion. All rights reserved.

#include "OrientationTrigger.h"
#include "CyberShooterPlayer.h"

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

AOrientationTrigger::AOrientationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a collision box to generate overlap events
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("OrientationCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AOrientationTrigger::BeginOverlap);
	RootComponent = Collision;

	// Create arrow components
	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrowComponent"));
	UpArrow->SetupAttachment(RootComponent);
	UpArrow->ArrowColor = FColor::Blue;
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrowComponent"));
	ForwardArrow->SetupAttachment(RootComponent);
	ForwardArrow->ArrowColor = FColor::Red;
	RequireArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequireArrowComponent"));
	RequireArrow->SetupAttachment(RootComponent);
	RequireArrow->ArrowColor = FColor::Yellow;
	RequireArrow->ArrowSize = 0.5f;

	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);
	RestrictOrientation = true;
	RequiredUp = FVector(0.0f, 0.0f, 1.0f);

	SetArrows();
}

void AOrientationTrigger::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SetArrows();
}

void AOrientationTrigger::PostLoad()
{
	Super::PostLoad();

	SetArrows();
}

void AOrientationTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Skip the orientation change if we are already in the proper orientation
		if (player->GetUpVector().GetSafeNormal().Equals(Up.GetSafeNormal()) && player->GetForwardVector().GetSafeNormal().Equals(Forward.GetSafeNormal()))
		{
			return;
		}

		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->GetUpVector().GetSafeNormal().Equals(RequiredUp.GetSafeNormal()))
			{
				return;
			}
		}

		player->SetOrientation(Forward.GetSafeNormal(), Up.GetSafeNormal());
	}
}

void AOrientationTrigger::SetArrows()
{
	UpArrow->SetWorldRotation(Up.Rotation());
	ForwardArrow->SetWorldRotation(Forward.Rotation());
	if (RestrictOrientation)
	{
		RequireArrow->SetWorldRotation(RequiredUp.Rotation());
		RequireArrow->SetVisibility(true);
	}
	else
	{
		RequireArrow->SetVisibility(false);
	}
}