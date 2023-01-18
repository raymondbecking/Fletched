// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "FletchedProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

// Declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam( FOnFire, AFletchedProjectile, OnFire, float, Multiplier );

UCLASS(config=Game)
class AFletchedProjectile : public AActor
{
	GENERATED_BODY()

protected:

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovement;
	

public:
	/** Scene Root */
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* SceneRoot = nullptr;
	
	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UBoxComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly)
	FCollisionProfileName CollisionProfile;

	UPROPERTY(EditDefaultsOnly)
	FVector ProjectileSize = FVector(40,1,1);
	
	AFletchedProjectile();

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintCallable, Category = "Fire")
	FOnFire OnFire;

	UFUNCTION()
	virtual void Fire(float Multiplier);
	
	/** called when projectile hits something */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
};
