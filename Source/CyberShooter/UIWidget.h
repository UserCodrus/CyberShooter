// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIWidget.generated.h"

// The main UI widget for the HUD
UCLASS()
class CYBERSHOOTER_API UUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Rebuild the widget to reflect changes to max hp and mp
	UFUNCTION(BlueprintImplementableEvent)
		void Rebuild();
	// Refresh the display to reflect damage taken and lost momentum
	UFUNCTION(BlueprintImplementableEvent)
		void Refresh();

	// Player status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Momentum;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float MaxMomentum;
};
