// Fill out your copyright notice in the Description page of Project Settings.


#include "FletchedCharacter.h"

// Sets default values
AFletchedCharacter::AFletchedCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFletchedCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AFletchedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


