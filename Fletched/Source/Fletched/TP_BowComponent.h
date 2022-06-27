// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_BowComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLETCHED_API UTP_BowComponent : public UTP_WeaponComponent
{
	GENERATED_BODY()

public:
	UTP_BowComponent();

protected:
	virtual void ChargeFire() override;

	virtual void ReleaseChargedFire() override;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	float CalculateProjectileSpeed();

	UPROPERTY()
	float ChargeStartTime;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	float MinChargeProjectileSpeed = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	float MaxChargeProjectileSpeed = 7000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	float MinChargeTimeInSeconds = .2f;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	float MaxChargeTimeInSeconds = 1.5f;
};
