// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CyberShooterPawn.generated.h"

UCLASS(Blueprintable)
class ACyberShooterPawn : public APawn
{
	GENERATED_BODY()

public:
	ACyberShooterPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Accessor functions
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return MeshComponent; }
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArmComponent; }

	// The function that handles the ship hitting obstacles
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Fire a shot in the given direction
	void FireShot(FVector FireDirection);
	// Callback for when the shot timer expires
	void ShotTimerExpired();

	// The offset for spawning projectiles
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
		FVector GunOffset;
	// The time between shots
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;
	// The speed that the ship moves
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;
	// The force mutiplier for collisions
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float CollisionForce = 2000.0f;

	// The projectile to spawn when firing
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> ProjectileClass;

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

protected:
	/// Components ///

	// The collision capsule
	UPROPERTY(Category = Component, VisibleDefaultsOnly, BlueprintReadOnly)
		class UCapsuleComponent* CollisionComponent;
	// The character's static mesh
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* MeshComponent;
	// The game camera
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComponent;

	/* Flag to control firing  */
	uint32 bCanFire : 1;
	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
};

