// Copyright Epic Games, Inc. All Rights Reserved.

#include "FletchedProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AFletchedProjectile::AFletchedProjectile() 
{
	OnFire.AddDynamic(this, &AFletchedProjectile::Fire);

	
	// Set as root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = SceneRoot;
	
	// Use a box as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->SetupAttachment(SceneRoot);
	
	CollisionComp->InitBoxExtent(ProjectileSize);

	//Manually set CollisionProfile instead of selectable from editor, editor set value can return "None"
	CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));	
	CollisionComp->OnComponentHit.AddDynamic(this, &AFletchedProjectile::OnHit);		// set up a notification for when this component hits something blocking
			
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	//Offset collider to match arrow
	FVector InitialPos = FVector(42.f,-2.f,7.f);
	FTransform InitialTransform = FTransform();
	InitialTransform.SetTranslation(InitialPos);
	CollisionComp->SetRelativeTransform(InitialTransform);
	
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	
	ProjectileMovement->bSimulationEnabled = false;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;

	// Set lifespan later when the arrow has been fired
	InitialLifeSpan = 0.0f;

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *CollisionComp->BodyInstance.GetCollisionProfileName().ToString());
}

//Behaviour when arrow OnFire event is triggered 
void AFletchedProjectile::Fire(float Multiplier)
{	
	//Prepare arrow for launch
	SetLifeSpan(80.f);	

	//Launch arrow
	ProjectileMovement->bSimulationEnabled = true;
	ProjectileMovement->bRotationFollowsVelocity = true;
	
	const FVector Force = GetActorForwardVector() * Multiplier;
	ProjectileMovement->AddForce(Force);
}

void AFletchedProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{	
	// Only add impulse and destroy projectile if we hit a physics
	if (OtherActor == nullptr || OtherActor == this || OtherComp == nullptr)
	{
		return;
	}
	
	//Behavior for Physics objects being hit
	if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * .3f, GetActorLocation());
		//Attach this arrow to a physics actor
		this->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
	}

	//Behaviour for Characters being hit
	if (OtherComp->IsA(USkeletalMeshComponent::StaticClass()))
	{
		//Basic Dismemberment TODO: Move to Fletched Character/Enemy and cut up the skeletalmesh in different pieces
		/*USkeletalMeshComponent* HitMesh = Cast<USkeletalMeshComponent>(OtherComp);
		HitMesh->BreakConstraint(NormalImpulse, Hit.Location, Hit.BoneName);*/

		//Attach projectile to a character
		ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
		if (HitCharacter != nullptr)
		{
			this->AttachToComponent(HitCharacter->GetMesh(), FAttachmentTransformRules::KeepWorldTransform,
			                        Hit.BoneName);
		}
		UE_LOG(LogTemp, Warning, TEXT("Try to Damage"));
		//Damage the actor that was hit by this projectile
		AActor* HitActor = Hit.GetActor();
		if (HitActor == nullptr)// || GetInstigator() == nullptr)
		{
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Creating Damage Event"));
		const FPointDamageEvent DamageEvent(Damage, Hit, FVector::One(), nullptr);
		if (AController* OwnerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); OwnerController != nullptr)
		{
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
			UE_LOG(LogTemp, Warning, TEXT("Damage ? %f"), Damage);
		}
	}

	//Disable arrow collision after hitting something
	this->SetActorEnableCollision(false);
	
}