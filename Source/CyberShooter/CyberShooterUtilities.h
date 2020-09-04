// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CyberShooterUtilities.generated.h"

class UArrowComponent;

UCLASS()
class UOrientationComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UOrientationComponent();

	void AlignArrows(FVector Up, FVector Forward, FVector Require);

protected:
	// Arrow components
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* UpArrow;
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* ForwardArrow;
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* RequireArrow;
};