// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CyberShooterHUD.generated.h"

// The core game HUD
UCLASS()
class CYBERSHOOTER_API ACyberShooterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

	// Set to true to draw debug text
	UPROPERTY(EditAnywhere)
		bool DrawDebug = true;
};
