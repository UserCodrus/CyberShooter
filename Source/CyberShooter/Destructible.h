// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Breakable.h"

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Destructible.generated.h"

UCLASS(Blueprintable)
class CYBERSHOOTER_API ADestructible : public AStaticMeshActor, public IBreakable
{
	GENERATED_BODY()
	
public:	
	ADestructible();

	/// Breakable Interface ///
	
	virtual void Damage(int32 Value);
	virtual void Heal(int32 Value);
	virtual void Kill();

protected:
	// The maximum health of the object
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		int32 MaxHealth;
	// The object's current health
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		int32 Health;

	// Particles that spawn when killed
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		UParticleSystem* DeathParticles;
	// The sound that plays when killed
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		USoundBase* DeathSound;
};
