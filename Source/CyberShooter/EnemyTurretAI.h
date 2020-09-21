// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyTurretAI.generated.h"

class ACyberShooterEnemy;

// Enemy AI that sits in place and aims at nearby players
UCLASS()
class CYBERSHOOTER_API AEnemyTurretAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyTurretAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	// The pawn being controlled by this AI
	UPROPERTY(VisibleAnywhere)
		ACyberShooterEnemy* EnemyPawn;
};
