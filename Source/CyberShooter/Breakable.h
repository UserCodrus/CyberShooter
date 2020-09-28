// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Breakable.generated.h"

class AActor;

// Enum to identify damage types in the editor
UENUM(meta = (Bitflags))
enum class EDamageTypeFlag : int32
{
	PLAYER,
	ENEMY,
	ENVIRONMENT,
	PHYSICS,
	EXPLOSION
};

// Enum to identify damage types
enum EDamageType : int32
{
	DAMAGETYPE_NONE = 0,
	DAMAGETYPE_PLAYER = 0b0001,
	DAMAGETYPE_ENEMY = 0b0010,
	DAMAGETYPE_ENVIRONMENT = 0b0100,
	DAMAGETYPE_PHYSICS = 0b1000,
	DAMAGETYPE_EXPLOSION = 0b10000
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBreakable : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects that can be damaged and destroyed
class CYBERSHOOTER_API IBreakable
{
	GENERATED_BODY()

public:
	// Damaging the object
	UFUNCTION()
		virtual void Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin) = 0;
	// Heal the object
	UFUNCTION()
		virtual void Heal(int32 Value) = 0;
	// Called when the object is killed
	UFUNCTION()
		virtual void Kill() = 0;
};
