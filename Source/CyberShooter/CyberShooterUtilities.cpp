// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterUtilities.h"

#include "Components/ArrowComponent.h"

UOrientationComponent::UOrientationComponent()
{
	// Create arrow components
	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrowComponent"));
	UpArrow->SetAbsolute(false, true, true);
	UpArrow->SetupAttachment(this);
	UpArrow->ArrowColor = FColor::Blue;
	UpArrow->bIsEditorOnly = true;

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrowComponent"));
	ForwardArrow->SetAbsolute(false, true, true);
	ForwardArrow->SetupAttachment(this);
	ForwardArrow->ArrowColor = FColor::Red;
	ForwardArrow->bIsEditorOnly = true;

	RequireArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequireArrowComponent"));
	RequireArrow->SetAbsolute(false, true, true);
	RequireArrow->SetupAttachment(this);
	RequireArrow->ArrowColor = FColor::Yellow;
	RequireArrow->bIsEditorOnly = true;
}

void UOrientationComponent::AlignArrows(FVector Up, FVector Forward, FVector Require)
{
	if (FVector::Orthogonal(Up, Forward) && !Up.IsZero() && !Forward.IsZero())
	{
		// Set the orientation arrows to match the orientation vectors
		UpArrow->SetWorldRotation(Up.Rotation());
		UpArrow->SetVisibility(true);
		ForwardArrow->SetWorldRotation(Forward.Rotation());
		ForwardArrow->SetVisibility(true);

		if (!Require.IsZero())
		{
			// Move the arrow adjacent to the orientation arrows
			FVector cross = FVector::CrossProduct(Up, Forward);
			RequireArrow->SetRelativeLocation(cross * 10.0f);

			// Set the arrow to match the requirement vector
			RequireArrow->SetWorldRotation(Require.Rotation());
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
		RequireArrow->SetVisibility(false);
	}
}