// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"

#include "BSP.h"
#include "DrawDebugHelpers.h"
#include "BSP3d.h"
#include "BSP3dNode.h"
#include "PlayerCharacter.h"

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

	//Bind Generate Dungeon Room to the F button for debug
	Character = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if(Character == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Not working"));
		return;
	}
	Character->OnUsePrimarySkill.AddDynamic(this, &AProceduralRoom::GenerateRoom);
}

void AProceduralRoom::GenerateRoom()
{
	//Temporary flush debug lines for debugging
	FlushPersistentDebugLines(GetWorld());

	// 2D dungeon
	//TSharedPtr<BSP3d> TheRoom(new BSP());

	// 3D dungeon
	TSharedPtr<BSP3d> TheRoom(new BSP3d());
	TheRoom->Partition();

	TheRoom->DrawBSPNodes(GetWorld());

	//UE_LOG(LogTemp, Warning, TEXT("Number of nodes in partitioned floor stack: %d"), TheRoom->GetPartitionedNodes().Num());
	//UE_LOG(LogTemp, Warning, TEXT("Number of nodes in partitioned floor stack: %d"), TheRoom->GetPartitionedBSPNodes().Num());
}

// Called every frame
void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

