// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"
#include "DrawDebugHelpers.h"
#include "BSP3d.h"
#include "BSP3dNode.h"

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

	TSharedPtr<BSP3d> TheFloor(new BSP3d());
	TheFloor->Partition();

	TheFloor->DrawBSPNodes(GetWorld());

	UE_LOG(LogTemp, Warning, TEXT("Number of nodes in partitioned floor stack: %d"), TheFloor->GetPartitionedNodes().Num());
}

// Called every frame
void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

