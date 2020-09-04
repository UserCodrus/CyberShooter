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
	UpArrow->SetAbsolute(false, true, true);
	UpArrow->SetupAttachment(RootComponent);
	UpArrow->ArrowColor = FColor::Blue;
	UpArrow->bIsEditorOnly = true;
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrowComponent"));
	ForwardArrow->SetAbsolute(false, true, true);
	ForwardArrow->SetupAttachment(RootComponent);
	ForwardArrow->ArrowColor = FColor::Red;
	UpArrow->bIsEditorOnly = true;
	RequireArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequireArrowComponent"));
	RequireArrow->SetAbsolute(false, true, true);
	RequireArrow->SetupAttachment(RootComponent);
	RequireArrow->ArrowColor = FColor::Yellow;
	UpArrow->bIsEditorOnly = true;

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
	if (UpArrow == nullptr || ForwardArrow == nullptr || RequireArrow == nullptr)
		return;

	if (FVector::Orthogonal(Up, Forward))
	{
		// Set the orientation arrows to match the orientation vectors
		UpArrow->SetWorldRotation(Up.Rotation());
		UpArrow->SetVisibility(true);
		ForwardArrow->SetWorldRotation(Forward.Rotation());
		ForwardArrow->SetVisibility(true);

		if (RestrictOrientation)
		{
			// Move the arrow adjacent to the orientation arrows
			FVector cross = FVector::CrossProduct(Up, Forward);
			RequireArrow->SetRelativeLocation(cross * 10.0f);

			// Set the arrow to match the requirement vector
			RequireArrow->SetWorldRotation(RequiredUp.Rotation());
			RequireArrow->SetVisibility(true);
		}
		else
		{
			RequireArrow->SetVisibility(false);
		}
	}
	else
	{
		// Hide the arrows if they are not orthogonal
		UpArrow->SetVisibility(false);
		ForwardArrow->SetVisibility(false);
	}
}