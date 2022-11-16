// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoom.generated.h"

class APlayerCharacter;

UCLASS()
class FLETCHED_API AProceduralRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void GenerateRoom();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Room, meta = (AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> FloorMesh;

	APlayerCharacter* Character;
};
