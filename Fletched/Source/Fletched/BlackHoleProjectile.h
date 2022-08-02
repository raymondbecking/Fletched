// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FletchedProjectile.h"
#include "BlackHoleProjectile.generated.h"

/**
 * 
 */
UCLASS()
class FLETCHED_API ABlackHoleProjectile : public AFletchedProjectile
{
	GENERATED_BODY()

	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=BlackHoleProjectile)
	UBoxComponent* DetectorComp;

	/** Maximum object thickness in cm the projectile can teleport through **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float MaxPierceThickness = 500.f;

	/** How far from the entry point the black hole should appear
	  *(Note: this does not have an effect on the distance from where the projectile teleports,
	  *This is decided by position and size of the detector component)**/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float TeleportEntryMargin = 0.f;
	
	/** How far from the exit point the projectile should teleport **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float TeleportExitMargin = 30.f;

    /** Teleports projectile past the actor if the reverse LineTrace hits the passed actor **/
	UFUNCTION()
	void TeleportProjectile(AActor* TeleportActor);

	/** Effect to spawn **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	TSubclassOf<AActor> BlackHoleEffectClass;

	/** Spawns Black Hole at specified transform **/
	UFUNCTION()
	void SpawnBlackHole(const FTransform SpawnTransform);

	/** Time until the black hole should disappear
	 * TODO: Extend BlackHole FX timeline based on this value **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float BlackHoleLifeSpan = .5f;
	
public:
	ABlackHoleProjectile();

	/** called when projectile hits something */
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit) override;


	/** Called when projectile is about to hit something */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

protected:
	virtual void BeginPlay();
};
