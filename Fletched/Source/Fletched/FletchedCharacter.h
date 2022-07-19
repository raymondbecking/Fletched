// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FletchedCharacter.generated.h"

UCLASS()
class FLETCHED_API AFletchedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFletchedCharacter();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 30.f;

	UPROPERTY(VisibleAnywhere)
	float Health = 100.f;

	UPROPERTY(EditDefaultsOnly)
	FCollisionProfileName OnDeathCollisionProfile;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};