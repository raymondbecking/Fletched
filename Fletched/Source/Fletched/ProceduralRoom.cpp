// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"
#include "DrawDebugHelpers.h"
#include "Floor.h"
#include "FloorNode.h"

// Sets default values
AProceduralRoom::AProceduralRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	SetRootComponent(FloorMesh);	
}

// Called when the game starts or when spawned
void AProceduralRoom::BeginPlay()
{
	Super::BeginPlay();

	TSharedPtr<Floor> TheFloor(new Floor());
	TheFloor->Partition();

	TheFloor->DrawFloorNodes(GetWorld());

	UE_LOG(LogTemp, Warning, TEXT("Number of nodes in partitioned floor stack: %d"), TheFloor->GetPartitionedFloor().Num());
}

// Called every frame
void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

