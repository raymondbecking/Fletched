// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_BowComponent.h"
#include "FletchedProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetTextLibrary.h"


UTP_BowComponent::UTP_BowComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(-250.0f, 0.0f, 10.0f);
}

void UTP_BowComponent::ChargeFire()
{
	Super::ChargeFire();
	NockArrow(true);
}

void UTP_BowComponent::ReleaseChargedFire()
{
	//TODO: Swap order?
	SpawnArrow();
	NockArrow(false);
}



void UTP_BowComponent::SpawnArrow()
{
	if (ProjectileClass == nullptr || GetOwner() == nullptr)
	{
		return;
	}
	UWorld* const World = GetWorld();
	if (World == nullptr && !bEnableChargedFire)
	{
		return;
	}

	const float ProjectileSpeed = ChargedProjectileSpeed();
	//Abort charged fire if projectile speed is invalid
	if (ProjectileSpeed == 0.f) { return; }

	FTransform ProjectileTransform;
	ProjectileTransform.SetLocation(SpawnLocation);
	ProjectileTransform.SetRotation(SpawnRotation.Quaternion());

	//Save pointer to the spawned projectile
	AFletchedProjectile* ProjectileActor = World->SpawnActorDeferred<AFletchedProjectile>(
		ProjectileClass, ProjectileTransform, GetOwner(), GetOwner()->GetAttachParentActor()->GetInstigator());


	//Access projectile pointer to change settings for spawned projectile
	if (ProjectileActor != nullptr)
	{
		ProjectileActor->GetProjectileMovement()->MaxSpeed = ProjectileSpeed;
		ProjectileActor->GetProjectileMovement()->InitialSpeed = ProjectileSpeed;
		UGameplayStatics::FinishSpawningActor(ProjectileActor, ProjectileTransform);
	}
		
	
}

void UTP_BowComponent::NockArrow(bool bIsNocking)
{
	if (ProjectileClass == nullptr)
	{
		return;
	}
	UWorld* const World = GetWorld();
	if (World == nullptr && !bEnableChargedFire)
	{
		return;
	}

	// Hide arrow from bowstring if there's an arrow attached
	if (!bIsNocking)
	{
		if (StaticArrow != nullptr)
		{
			// Hide Static arrow on bow when the projectile is fired
			StaticArrow->SetActorHiddenInGame(true);
		}
	}

	// Attach arrow to bowstring
	if (bIsNocking)
	{
		FString SocketMeshName = "recurve_Rope";
		FName SocketName = TEXT("ArrowSocket");

		// Only spawn StaticArrow the first time
		if (StaticArrow == nullptr && GetSocketMesh(SocketMeshName) != nullptr)
		{
			UStaticMeshComponent* SocketMesh = GetSocketMesh(SocketMeshName);

			// Spawn Arrow on the bowstring
			StaticArrow = World->SpawnActorDeferred<AFletchedProjectile>(
				ProjectileClass, SocketMesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Component));

			// Settings for newly spawned arrow
			if (StaticArrow != nullptr)
			{
				// Destroy ProjectileMovement since we do not need it on the static arrow attached to the bow
				StaticArrow->GetProjectileMovement()->DestroyComponent();
				// Prevent destruction of the projectile
				StaticArrow->SetLifeSpan(0);
				// Attach to the bowstring
				StaticArrow->AttachToComponent(SocketMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				                               SocketName);
				UGameplayStatics::FinishSpawningActor(StaticArrow,
				                                      SocketMesh->GetSocketTransform(
					                                      SocketName, ERelativeTransformSpace::RTS_Component));
			}
		}

		// On consecutive Nocking only set the StaticArrow to Visible
		if (StaticArrow != nullptr)
		{
			StaticArrow->SetActorHiddenInGame(false);
		}
	}	
}

float UTP_BowComponent::ChargedProjectileSpeed()
{
	float TotalChargeTime = TotalTimeCharged();

	//Abort bow charge if it wasn't held long enough
	if(TotalChargeTime < MinChargeTimeInSeconds)
	{
		return 0.f;
	}	
	//Make sure charge time does not exceed maximum
	if(TotalChargeTime > MaxChargeTimeInSeconds)
	{
		TotalChargeTime = MaxChargeTimeInSeconds;
	}

	//Normalize the amount of time the bow was charged
	const float NormalizedChargeTime = UKismetMathLibrary::NormalizeToRange(TotalChargeTime, MinChargeTimeInSeconds, MaxChargeTimeInSeconds);
	
	//Amount of speed that can be added by the charge time
	const float ProjectileSpeedRange = MaxChargeProjectileSpeed - MinChargeProjectileSpeed;
	
	//This gives the maximum projectile speed when the bow is fully charged
	const float ProjectileSpeed = NormalizedChargeTime * ProjectileSpeedRange;

	return ProjectileSpeed + MinChargeProjectileSpeed;
}

UStaticMeshComponent* UTP_BowComponent::GetSocketMesh(FString SocketMesh)
{
	//Get the owner static mesh where this component is attached to
	if(GetOwner() == nullptr)
	{
		return nullptr;
	}

	//Get all mesh components attached to the bow
	TArray<UStaticMeshComponent*> MeshComponents;
	GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);
	
	if (!MeshComponents.IsEmpty())
	{
		for(UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			//Mesh with socket to attach arrow to
			if(MeshComponent->GetName() == SocketMesh)
			{
				return MeshComponent;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Socket Mesh does not exist!"));
	return nullptr;		
}
