// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_BowComponent.h"
#include "FletchedProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetTextLibrary.h"


UTP_BowComponent::UTP_BowComponent()
{
	
}

void UTP_BowComponent::ChargeFire()
{
	Super::ChargeFire();

	//"Attach" Arrow to the bow
	NockArrow(true);	
}

void UTP_BowComponent::ReleaseChargedFire()
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if(bEnableChargedFire)
			{
				const float ProjectileSpeed = ChargedProjectileSpeed();
				//Abort charged fire if projectile speed is invalid
				if (ProjectileSpeed == 0.f) { return; }
				
				FTransform ProjectileTransform;
				ProjectileTransform.SetLocation(SpawnLocation);
				ProjectileTransform.SetRotation(SpawnRotation.Quaternion());

				//Save pointer to the spawned projectile
				AFletchedProjectile* ProjectileActor = World->SpawnActorDeferred<AFletchedProjectile>(
					ProjectileClass, ProjectileTransform);

				//Access projectile pointer to change settings for spawned projectile
				if (ProjectileActor != nullptr)
				{
					//"Release" the arrow from the bow
					NockArrow(false);
					
					ProjectileActor->GetProjectileMovement()->MaxSpeed = ProjectileSpeed;
					ProjectileActor->GetProjectileMovement()->InitialSpeed = ProjectileSpeed;
					UGameplayStatics::FinishSpawningActor(ProjectileActor, ProjectileTransform);
				}
			}	
	
		}
	}
}

void UTP_BowComponent::NockArrow(bool bArrowNocked)
{
	if (GetOwner() != nullptr)
	{
		//Get the static mesh components of the bow
		TArray<UStaticMeshComponent*> MeshComponents;
		GetOwner()->GetComponents<UStaticMeshComponent>(MeshComponents);

		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			if (MeshComponent->GetName() == "arrow")
			{
				if (bArrowNocked)
				{
					//Nock the arrow
					MeshComponent->SetVisibility(true);
				}
				else
				{
					//Release arrow
					MeshComponent->SetVisibility(false);
				}
			}
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

	UE_LOG(LogTemp, Warning, TEXT("Speed : %f"), ProjectileSpeed + MinChargeProjectileSpeed);
	return ProjectileSpeed + MinChargeProjectileSpeed;
}
