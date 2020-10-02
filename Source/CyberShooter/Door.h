// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "Door.generated.h"

// A barrier that can be removed using lock actors
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ADoor : public APhysicalStaticMesh, public ILockInterface
{
	GENERATED_BODY()
	
public:
	ADoor();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

	/// Door Functions ///
	
	// Open the door
	UFUNCTION(Category = "Door", BlueprintCallable, CallInEditor)
		void OpenDoor();
	// Close the door
	UFUNCTION(Category = "Door", BlueprintCallable, CallInEditor)
		void CloseDoor();

	// Returns true if the door is open
	UFUNCTION(BlueprintPure)
		bool IsOpen() const;

protected:
	/// Blueprint Events ///
	
	// Called when the door is opened
	UFUNCTION(BlueprintNativeEvent)
		void OnOpen();
	// Called when the door is closed
	UFUNCTION(BlueprintNativeEvent)
		void OnClose();

	/// Properties ///
	
	// Set to true after the door opens
	UPROPERTY(VisibleAnywhere)
		bool Open;

	// The sound played when the door opens
	UPROPERTY(EditAnywhere)
		USoundBase* OpenSound;
	// The sound played when the door closes
	UPROPERTY(EditAnywhere)
		USoundBase* CloseSound;
};
