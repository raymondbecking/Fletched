// Fill out your copyright notice in the Description page of Project Settings.


// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "FletchedProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	if (GetCharacterMovement() != nullptr)
	{
		//Save original friction settings, character friction is manipulated by sliding
		DefaultFriction = GetCharacterMovement()->GroundFriction;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning! CharacterMovement does NOT exist on this Character!"))
	}
}

void APlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::ActivateJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::DeactivateJump);
	
	// Bind sprint event
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	// Bind crouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::StopCrouch);

	// Bind slide event
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &APlayerCharacter::Slide);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::StopSlide);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APlayerCharacter::OnPrimaryAction);

	// Bind charge and release item event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APlayerCharacter::OnPrimaryHoldAction);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &APlayerCharacter::OnPrimaryReleaseAction);

	// Bind use skill event
	PlayerInputComponent->BindAction("PrimarySkill", IE_Released, this, &APlayerCharacter::OnPrimarySkill);
	

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void APlayerCharacter::OnPrimaryHoldAction()
{
	// Trigger the OnItemUsed Event
	OnHoldItem.Broadcast();
}

void APlayerCharacter::OnPrimaryReleaseAction()
{
	// Trigger the OnItemUsed Event
	OnReleaseItem.Broadcast();
}

void APlayerCharacter::OnPrimarySkill()
{
	if(GetWorld() != nullptr)
	{
		// Enable slowmotion if it wasn't already
		if(UGameplayStatics::GetGlobalTimeDilation(GetWorld()) != SlowMotionMultiplier)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionMultiplier);			
		}
		// Disable slowmotion if it was enabled
		else
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);			
		}
	}
}


void APlayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APlayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

/** Overridden CanJumpInternal to not disallow jumping from crouched state */
bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

void APlayerCharacter::ActivateJump()
{
	//Stop slide (and thus the Crouch state) to allow jumping from a crouched position
	APlayerCharacter::StopSlide();
	GetCharacterMovement()->bWantsToCrouch = false;

	ACharacter::Jump();
}

void APlayerCharacter::DeactivateJump()
{
	ACharacter::StopJumping();
}

void APlayerCharacter::Sprint()
{
	//Set movement speed to sprinting speed
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprint()
{	
	//Set movement speed to walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;	
}

void APlayerCharacter::StartCrouch()
{
	ACharacter::Crouch();
}

void APlayerCharacter::StopCrouch()
{
	ACharacter::UnCrouch();	
}

void APlayerCharacter::Slide()
{	
	//Directional velocity of the character
	FVector CharacterVelocity = GetCharacterMovement()->Velocity;
	CharacterVelocity.Normalize();

	//Dot product of the Directional Velocity and the Characters rotation to calculate the characters moving direction (forward/backward)
	float CharacterDirection = FVector::DotProduct(CharacterVelocity, this->GetActorForwardVector());
	//UE_LOG(LogTemp, Warning, TEXT("Moving Direction : %f"), CharacterDirection);

	//Allow sliding if character is moving forward
	if (CharacterDirection > 0)
	{
		SlidePhysics(true);

		//Event that adjusts camera roll smoothly to make character slide on its side
		if (StartCameraTilt.IsBound())
		{
			//Trigger Start Camera Tilt Event 
			StartCameraTilt.Broadcast();
		}
	}

	ACharacter::Crouch();
}

void APlayerCharacter::StopSlide()
{
	SlidePhysics(false);

	if (StopCameraTilt.IsBound())
	{
		//Trigger Stop Camera Tilt Event
		StopCameraTilt.Broadcast();
	}	

	ACharacter::UnCrouch();
}

void APlayerCharacter::SlidePhysics(bool bIsSliding)
{
	if(bIsSliding)
	{
		//Enable Sliding physics
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingDecelerationWalking = SlideDecelerationSpeed;
		GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	}
	else
	{
		//Return physics to normal
		GetCharacterMovement()->GroundFriction = DefaultFriction;
		GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
		GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = false;
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool APlayerCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APlayerCharacter::EndTouch);

		return true;
	}
	
	return false;
}

