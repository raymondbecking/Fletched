// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHoleProjectile.h"

#include "EnemyCharacter.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABlackHoleProjectile::ABlackHoleProjectile()
{	
	// Use a box as a simple collision representation
	DetectorComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Detector"));
	DetectorComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	DetectorComp->InitBoxExtent(FVector(1.5,1.5,1.5));
	DetectorComp->SetRelativeLocation(DetectorOffset);

	ProjectileSize = FVector(35.f, 5.f, 5.f);	
}

void ABlackHoleProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Set up a notification for when this component overlaps something
	DetectorComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHoleProjectile::OnOverlapBegin);
}

void ABlackHoleProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *OtherActor->GetName());
}


void ABlackHoleProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	if ((OtherActor == nullptr) || (OtherActor == this) || (OtherComp == nullptr))
	{
		return;
	}
	
	//Dont interact with characters
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (HitCharacter == nullptr)
	{
		TeleportProjectile();
	}
	else
	{
		//Check if the character hit was an enemy
		if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(OtherActor); HitEnemy != nullptr)
		{
			if (ProjectileMovement != nullptr)
			{
				//Enable sweep to be able to hit an enemy
				ProjectileMovement->bSweepCollision = true;
			}
		}
	}
	
}

void ABlackHoleProjectile::TeleportProjectile()
{
	FVector TraceStart = this->GetActorLocation();
	FVector TraceEnd = this->GetActorLocation() + this->GetActorForwardVector() * MaxPierceThickness;

	FHitResult LineHit;
	TArray<FHitResult> LineHitReverse;
	//Find what is about to be hit, ECC_GameTraceChannel1 = MultiLineIgnoreProjectile
	GetWorld()->LineTraceSingleByChannel(LineHit, TraceStart, TraceEnd, ECC_GameTraceChannel1);
	//Find all objects from MaxPierceThickness to the projectile
	GetWorld()->LineTraceMultiByChannel(LineHitReverse, TraceEnd, TraceStart, ECC_GameTraceChannel1);

	for (FHitResult ReverseHit : LineHitReverse)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reverse Hit Result: %s"), *ReverseHit.GetComponent()->GetName());
		// Make sure that the projectile only teleports past the actor that was hit
		if (ReverseHit.GetActor() == LineHit.GetActor())
		{
			DrawDebugLine(GetWorld(), ReverseHit.Location,
			              ReverseHit.Location + (this->GetActorForwardVector() * 200), FColor(255, 0, 0),
			              false, 60.f, 0, 5.f);

			if (ProjectileMovement != nullptr)
			{
				// Calculate where arrow should go using the line traces
				FVector TeleportDistance = ReverseHit.Location - LineHit.Location;
				FVector TeleportAmount = TeleportDistance + (this->GetActorForwardVector() * TeleportMargin);
				// Teleport arrow to new location
				ProjectileMovement->MoveUpdatedComponent(TeleportAmount, this->GetActorRotation(), false,
				                                         nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}
