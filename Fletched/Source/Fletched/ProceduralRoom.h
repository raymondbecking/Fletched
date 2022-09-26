// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoom.generated.h"

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

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Room, meta = (AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> Floor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Room, meta = (AllowPrivateAccess=true))
	TSubclassOf<AActor> ChairClass;
	
	void SpawnItem(TObjectPtr<UClass> ItemToSpawn);

	float SquareWidth;
	float GridHeight;
	float RoomLength;
	float RoomWidth;
	
	FVector TopLeft;
	FVector BottomRight;
	int32 GridSizeX;
	int32 GridSizeY;

	float Radius = 25.f;
	
	void CreateGrid();

	FVector GetRandomPointInSquare(const FVector& UpperLeft, const FVector& LowerRight);

	void PlacePointsOnGrid();
};
