// Fill out your copyright notice in the Description page of Project Settings.


#include "FletchedCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AFletchedCharacter::AFletchedCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Health = MaxHealth;
}

// Called every frame
void AFletchedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Generic Take Damage handler to apply damage to any FletchedCharacter (Player and Enemy)
float AFletchedCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//Apply damage and prevent going below 0 health
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;

	if(IsDead())
	{
		//Stop character controls
		DetachFromControllerPendingDestroy();

		//Use physics to ragdoll on death
		GetMesh()->SetSimulatePhysics(true);
		
		//Adjust collision to prevent strange ragdoll behaviour
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionProfileName(OnDeathCollisionProfile.Name);		
	}
	return DamageToApply;
}

//Blueprint Callable
bool AFletchedCharacter::IsDead() const
{
	return Health <= 0;
}


