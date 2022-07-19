// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void AEnemyCharacter::DeathBehaviour()
{
	Super::DeathBehaviour();
	
	//Use physics to ragdoll on death
	GetMesh()->SetSimulatePhysics(true);

	//Set mesh anim class to null to prevent an array out of index error related to the ragdoll
	GetMesh()->AnimClass = nullptr;
		
	//Adjust collision to prevent strange ragdoll behaviour
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(OnDeathCollisionProfile.Name);		
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

