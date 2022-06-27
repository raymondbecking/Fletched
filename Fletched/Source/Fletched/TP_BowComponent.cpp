// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_BowComponent.h"
#include "FletchedProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"


UTP_BowComponent::UTP_BowComponent()
{
	
}

void UTP_BowComponent::ChargeFire()
{
	if(GetWorld() != nullptr)
	{
		ChargeStartTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());	
	}
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
				//Return if projectile speed is invalid
				if (CalculateProjectileSpeed() == 0.f) { return; }
				
				FTransform ProjectileTransform;
				ProjectileTransform.SetLocation(SpawnLocation);
				ProjectileTransform.SetRotation(SpawnRotation.Quaternion());

				//Save pointer to the spawned projectile
				AFletchedProjectile* ProjectileActor = World->SpawnActorDeferred<AFletchedProjectile>(
					ProjectileClass, ProjectileTransform);

				//Settings for spawned projectile
				if (ProjectileActor != nullptr)
				{					
					ProjectileActor->GetProjectileMovement()->MaxSpeed = CalculateProjectileSpeed();
					ProjectileActor->GetProjectileMovement()->InitialSpeed = CalculateProjectileSpeed();
					UGameplayStatics::FinishSpawningActor(ProjectileActor, ProjectileTransform);
				}
			}	
	
		}
	}
}

float UTP_BowComponent::CalculateProjectileSpeed()
{
	if (GetWorld() == nullptr)
	{
		return 0.f;
	}

	//Calculate how long the bow was charged in seconds
	const float ChargeEndTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	float TotalChargeTime = ChargeEndTime - ChargeStartTime;
	UE_LOG(LogTemp, Warning, TEXT("Charge time : %f seconds"), TotalChargeTime);

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
	float NormalizedChargeTime = UKismetMathLibrary::NormalizeToRange(TotalChargeTime, MinChargeTimeInSeconds, MaxChargeTimeInSeconds);
	
	//Amount of speed that can be added by the charge time
	float ProjectileSpeedRange = MaxChargeProjectileSpeed - MinChargeProjectileSpeed;
	
	//This gives the maximum projectile speed when the bow is fully charged
	float ProjectileSpeed = NormalizedChargeTime * ProjectileSpeedRange;

	UE_LOG(LogTemp, Warning, TEXT("Speed : %f"), ProjectileSpeed + MinChargeProjectileSpeed);
	return ProjectileSpeed + MinChargeProjectileSpeed;
}
