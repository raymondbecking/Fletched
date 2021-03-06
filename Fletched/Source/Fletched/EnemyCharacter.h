// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FletchedCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class FLETCHED_API AEnemyCharacter : public AFletchedCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when this character dies
	virtual void DeathBehaviour() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
