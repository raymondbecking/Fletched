// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

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

void AFletchedProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//Behavior for Physics objects being hit
		if(OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * .3f, GetActorLocation());
			//Attach this arrow to a physics actor
			this->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
		}

		//Behaviour for Characters being hit
		if(OtherComp->IsA(USkeletalMeshComponent::StaticClass()))
		{
			//Basic Dismemberment TODO: Move to Fletched Character/Enemy and cut up the skeletalmesh in different pieces
			/*USkeletalMeshComponent* HitMesh = Cast<USkeletalMeshComponent>(OtherComp);
			HitMesh->BreakConstraint(NormalImpulse, Hit.Location, Hit.BoneName);*/

			//Attach projectile to a character
			ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
			if(HitCharacter != nullptr)
			{			
				this->AttachToComponent(HitCharacter->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);				
			}

			//Damage the actor that was hit by this projectile
			AActor* HitActor = Hit.GetActor();
			if(HitActor == nullptr || GetInstigator() == nullptr)
			{
				return;				
			}
			const FPointDamageEvent DamageEvent(Damage, Hit, FVector::One(), nullptr);
			if(AController* OwnerController = this->GetInstigator()->GetController(); OwnerController != nullptr)
			{
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);						
			}			
		}

		//Disable arrow collision after hitting something
		this->SetActorEnableCollision(false);
	}
}
