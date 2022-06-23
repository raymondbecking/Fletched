// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedCharacter.h"
#include "FletchedProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AFletchedCharacter

AFletchedCharacter::AFletchedCharacter()
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

void AFletchedCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void AFletchedCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFletchedCharacter::ActivateJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFletchedCharacter::DeactivateJump);
	
	// Bind sprint event
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFletchedCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFletchedCharacter::StopSprint);

	// Bind crouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFletchedCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFletchedCharacter::StopCrouch);

	// Bind slide event
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AFletchedCharacter::Slide);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFletchedCharacter::StopSlide);


	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFletchedCharacter::OnPrimaryAction);

	// Bind charge and release item event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFletchedCharacter::OnPrimaryHoldAction);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &AFletchedCharacter::OnPrimaryReleaseAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFletchedCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFletchedCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFletchedCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFletchedCharacter::LookUpAtRate);
}

void AFletchedCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AFletchedCharacter::OnPrimaryHoldAction()
{
	// Trigger the OnItemUsed Event
	OnHoldItem.Broadcast();
}

void AFletchedCharacter::OnPrimaryReleaseAction()
{
	// Trigger the OnItemUsed Event
	OnReleaseItem.Broadcast();
}

void AFletchedCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
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

void AFletchedCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AFletchedCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFletchedCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

/** Overridden CanJumpInternal to not disallow jumping from crouched state */
bool AFletchedCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

void AFletchedCharacter::ActivateJump()
{
	//Stop slide (and thus the Crouch state) to allow jumping from a crouched position
	AFletchedCharacter::StopSlide();
	GetCharacterMovement()->bWantsToCrouch = false;

	ACharacter::Jump();
}

void AFletchedCharacter::DeactivateJump()
{
	ACharacter::StopJumping();
}

void AFletchedCharacter::Sprint()
{
	//Set movement speed to sprinting speed
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AFletchedCharacter::StopSprint()
{	
	//Set movement speed to walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;	
}

void AFletchedCharacter::StartCrouch()
{
	ACharacter::Crouch();
}

void AFletchedCharacter::StopCrouch()
{
	ACharacter::UnCrouch();	
}

void AFletchedCharacter::Slide()
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

void AFletchedCharacter::StopSlide()
{
	SlidePhysics(false);

	if (StopCameraTilt.IsBound())
	{
		//Trigger Stop Camera Tilt Event
		StopCameraTilt.Broadcast();
	}	

	ACharacter::UnCrouch();
}

void AFletchedCharacter::SlidePhysics(bool bIsSliding)
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

void AFletchedCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AFletchedCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AFletchedCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFletchedCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFletchedCharacter::EndTouch);

		return true;
	}
	
	return false;
}
