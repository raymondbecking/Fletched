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

	/** Distance from where an object to teleport through should be detected **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	FVector DetectorOffset = FVector(60,1,1);

	/** Maximum object thickness the projectile can teleport through **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float MaxPierceThickness = 200.f;

	/** How far from the exit point the projectile should teleport **/
	UPROPERTY(EditDefaultsOnly, Category=BlackHoleProjectile)
	float TeleportMargin = 120.f;
	
public:
	ABlackHoleProjectile();

	/** called when projectile hits something */
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit) override;
	void TeleportProjectile();


	/** Called when projectile is about to hit something */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

protected:
	virtual void BeginPlay();
};
