// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AFletchedProjectile::AFletchedProjectile() 
{
	// Use a box as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));

	//TODO: Calculate collider size based on the static mesh ?
	CollisionComp->InitBoxExtent(ProjectileSize);
	CollisionComp->BodyInstance.SetCollisionProfileName(CollisionProfile.Name);	
	CollisionComp->OnComponentHit.AddDynamic(this, &AFletchedProjectile::OnHit);		// set up a notification for when this component hits something blocking
		
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 60 seconds by default
	InitialLifeSpan = 60.0f;
}

void AFletchedProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		/*//Behavior for arrows hitting arrows
		if(OtherActor->IsA(AFletchedProjectile::StaticClass()))
		{
			//Replace the other arrow with this arrow if they collide (Robin Hood)
			this->SetActorTransform(OtherActor->GetTransform());

			//Check if arrow was attached to something
			if(OtherActor->GetOwner() != nullptr)
			{
				this->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
			}
			
			OtherActor->Destroy();
		}*/

		//Behavior for Physics objects being hit
		if(OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 1.0f, GetActorLocation());

			//Attach this arrow to a physics actor
			this->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}