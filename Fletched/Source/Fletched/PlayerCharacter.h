// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FletchedCharacter.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoldItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleaseItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUsePrimarySkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseDebugAction);
// Declaration of the delegate that will be called when Slide is triggered
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartCameraTilt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopCameraTilt);

UCLASS(config=Game)
class FLETCHED_API APlayerCharacter : public AFletchedCharacter
{
	GENERATED_BODY()
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnHoldItem OnHoldItem;
	
	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnReleaseItem OnReleaseItem;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUsePrimarySkill OnUsePrimarySkill;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Debug")
	FOnUseDebugAction OnUseDebugAction;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Slide")
	FStartCameraTilt StartCameraTilt;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Slide")
	FStopCameraTilt StopCameraTilt;

protected:
	
	/** Activates primary action. */
	void OnPrimaryAction();

	/** Holds primary action. */
	void OnPrimaryHoldAction();

	/** Release primary action. */
	void OnPrimaryReleaseAction();

	/** Release primary action. */
	void OnPrimarySkill();

	/** Release primary action. */
	void OnDebugAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Handles Jumping */
	void ActivateJump();

	/** Handles Jump cancel */
	void DeactivateJump();

	/** Handles sprinting */
	void Sprint();

	/** Handles sprint cancel */
	void StopSprint();

	/** Handles crouching */
	void StartCrouch();

	/** Handles uncrouching */
	void StopCrouch();

	/** Handles sliding */
	void Slide();

	/** Handles slide cancel */
	void StopSlide();

	/** Enables or Disables slide physics */
	void SlidePhysics(bool bIsSliding);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

private:
	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float SprintSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float SlideDecelerationSpeed = 600.f;

	UPROPERTY()
	float DefaultFriction;

	UPROPERTY(EditDefaultsOnly)
	float SlowMotionMultiplier = .33f;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Overrides CanJump implementation */
	virtual bool CanJumpInternal_Implementation() const override;

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
