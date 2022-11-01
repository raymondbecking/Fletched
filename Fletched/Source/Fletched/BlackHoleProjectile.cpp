// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHoleProjectile.h"

#include "EnemyCharacter.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABlackHoleProjectile::ABlackHoleProjectile()
{	
	// Use a box as a simple collision representation
	DetectorComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Detector"));
	DetectorComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DetectorComp->InitBoxExtent(FVector(1.5,1.5,1.5));
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
}

void ABlackHoleProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this || OtherComp == nullptr || ProjectileMovement == nullptr)
	{
		return;
	}
	
	//Non-Character Interactions
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (HitCharacter == nullptr)
	{
		// Only teleport when sweep is disabled
		if(ProjectileMovement->bSweepCollision == false)
		{
			TeleportProjectile(OtherActor);			
		}
	}
	else //Enemy Interactions
	{
		//Check if the character hit was an enemy
		if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(OtherActor); HitEnemy == nullptr)
		{
			return;
		}
		if (ProjectileMovement == nullptr)
		{
			return;
		}
		//Enable sweep to be able to hit an enemy
		ProjectileMovement->bSweepCollision = true;		
	}
	
}

void ABlackHoleProjectile::TeleportProjectile(AActor* TeleportActor)
{
	if (ProjectileMovement == nullptr)
	{
		return;
	}
	FVector TraceStart = this->GetActorLocation();
	FVector TraceEnd = this->GetActorLocation() + this->GetActorForwardVector() * MaxPierceThickness;

	FHitResult LineHit;
	TArray<FHitResult> LineHitReverse;
	//Find what is about to be hit
	GetWorld()->LineTraceSingleByChannel(LineHit, TraceStart, TraceEnd, ECC_Visibility);
	//Find all objects from MaxPierceThickness to the projectile	
	GetWorld()->LineTraceMultiByChannel(LineHitReverse, TraceEnd, TraceStart, ECC_Visibility);

	for (FHitResult ReverseHit : LineHitReverse)
	{
		// Make sure that the projectile only teleports past the actor that was hit
		if (ReverseHit.GetActor() == TeleportActor)
		{
			DrawDebugLine(GetWorld(), ReverseHit.Location,
				  ReverseHit.Location + (this->GetActorForwardVector() * 50), FColor(255, 0, 0),
				  false, 10.f, 0, 5.f);
			SpawnBlackHole(FTransform(LineHit.Location - (this->GetActorForwardVector() * TeleportEntryMargin)));

			// Represents the distance from the entry point to the exit point of an object
			FVector TeleportDistance = ReverseHit.Location - LineHit.Location;
			// Calculate where projectile should go including the exit margin
			FVector TeleportAmount = TeleportDistance + (this->GetActorForwardVector() * TeleportExitMargin);
			// Teleport projectile to new location
			ProjectileMovement->MoveUpdatedComponent(TeleportAmount, this->GetActorRotation(), false,
			                                         nullptr, ETeleportType::TeleportPhysics);

			SpawnBlackHole(FTransform(GetActorLocation()));
			
		}
	}
}

void ABlackHoleProjectile::SpawnBlackHole(const FTransform SpawnTransform)
{
	UWorld* World = GetWorld();
	if(World == nullptr || BlackHoleEffectClass == nullptr)
	{
		return;
	}
	
	AActor* BlackHoleActor = World->SpawnActorDeferred<AActor>(
		BlackHoleEffectClass, SpawnTransform, GetOwner(), GetOwner()->GetAttachParentActor()->GetInstigator());

	//Access projectile pointer to change settings for spawned projectile
	if (BlackHoleActor != nullptr)
	{
		BlackHoleActor->SetLifeSpan(BlackHoleLifeSpan);
		UGameplayStatics::FinishSpawningActor(BlackHoleActor, SpawnTransform);
	}
}
