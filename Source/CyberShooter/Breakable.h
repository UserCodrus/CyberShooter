// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Breakable.generated.h"

class AActor;

// Enum to identify damage types
UENUM(meta = (Bitflags))
enum class EDamageType
{
	GENERIC,
	PLAYER,
	ENEMY,
	ENVIRONMENT
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBreakable : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects tha can be destroyed
class CYBERSHOOTER_API IBreakable
{
	GENERATED_BODY()

public:
	// Function for damaging the object
	UFUNCTION()
		virtual void Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin) = 0;
	// Function for healing the object
	UFUNCTION()
		virtual void Heal(int32 Value) = 0;
	// Called when the object is killed
	UFUNCTION()
		virtual void Kill() = 0;
};
