// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrientationTrigger.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AOrientationTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrientationTrigger();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;

protected:
	// Called when the trigger overlaps an actor
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Set the orientation of arrow components
	void SetArrows();

	// The forward vector the trigger imparts
	UPROPERTY(Category = Orientation, EditAnywhere)
		FVector Forward;
	// The up vector the trigger will impart
	UPROPERTY(Category = Orientation, EditAnywhere)
		FVector Up;

	// If set to true, the trigger will only work from certain orientations
	UPROPERTY(Category = Orientation, EditAnywhere)
		bool RestrictOrientation;
	// The up vector required to use the trigger
	UPROPERTY(Category = Orientation, EditAnywhere)
		FVector RequiredUp;

	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* Collision;

	// Arrow components for convenience
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* UpArrow;
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* ForwardArrow;
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* RequireArrow;
};
