// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterPlayer.generated.h"

// A player controlled pawn with a camera and input setup
UCLASS(Blueprintable)
class CYBERSHOOTER_API ACyberShooterPlayer : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	ACyberShooterPlayer();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Kill();

	// Accessor functions
	FORCEINLINE class UCameraComponent* GetCamera() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArmComponent; }

protected:
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	// The game camera
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComponent;
};
