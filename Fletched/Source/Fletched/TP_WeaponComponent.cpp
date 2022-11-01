// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "PlayerCharacter.h"
#include "FletchedProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if(Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	if (ProjectileClass == nullptr)
	{
		return;
	}
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	// Try and fire a projectile
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (!bEnableChargedFire)
	{
		// Spawn the projectile at the muzzle
		World->SpawnActor<AFletchedProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
	else
	{
		ReleaseChargedFire();
	}	
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::ChargeFire()
{
	if(GetWorld() != nullptr)
	{
		ChargeStartTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());	
	}
}

void UTP_WeaponComponent::ReleaseChargedFire()
{
	//Release a charged shot
}


void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(Character != nullptr)
	{
		// Unregister from the OnUseItem Event
		Character->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
	}
}

void UTP_WeaponComponent::AttachWeapon(APlayerCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if(Character == nullptr)
	{
		return;
	}
	
	// Attach the weapon to the First Person Character
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	GetOwner()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	if (bEnableChargedFire)
	{
		// Register so that ChargeFire is called every time the character holds down the item being held
		Character->OnHoldItem.AddDynamic(this, &UTP_WeaponComponent::ChargeFire);
		// Register so that Fire is called every time the character tries to release the charge of the item being held
		Character->OnReleaseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
	}
	else
	{
		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
	}
	
}

float UTP_WeaponComponent::TotalTimeCharged()
{
	if (GetWorld() != nullptr)
	{
		//Calculate how long the bow was charged in seconds
		const float ChargeEndTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
		UE_LOG(LogTemp, Warning, TEXT("Charge time : %f seconds"), ChargeEndTime - ChargeStartTime);
		return ChargeEndTime - ChargeStartTime;
	}
	return 0.f;
}

