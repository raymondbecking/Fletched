#include "Floor.h"
#include "FloorNode.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "MathUtil.h"

Floor::Floor()
{
	FloorGridSizeX = 200;
	FloorGridSizeY = 200;
	RoomMinX = 20;
	RoomMinY = 20;

	UnitLength = 100.f;

	SplitChance = 1.3f;

	MinRoomSizePercent = 0.6f;

	HallwayMinWidth = 2;

	UE_LOG(LogTemp, Warning, TEXT("Floor created."));

}

Floor::~Floor()
{
	UE_LOG(LogTemp, Warning, TEXT("Floor destroyed."));
}

void Floor::Partition()
{
	FCornerCoordinates CornerCoordinatesA = {0,0, FloorGridSizeX, FloorGridSizeY};
	FloorNodeStack.Push(TSharedPtr<FloorNode>(new FloorNode(CornerCoordinatesA)));

	while (FloorNodeStack.Num() > 0)
	{
		TSharedPtr<FloorNode> A = FloorNodeStack.Pop();

		bool bNodeWasSplit = SplitAttempt(A);
		if(!bNodeWasSplit)
		{
			PartitionedFloor.Push(A);
		}
		
	}
}

int32 Floor::CoinFlip()
{
	return FMath::RandRange(0,1);
}

float Floor::DiceRoll()
{
	return FMath::FRandRange(0.f, 1.f);
}

bool Floor::ShouldSplitNode(TSharedPtr<FloorNode> InNode, ESplitOrientation Orientation)
{
	FCornerCoordinates CornerCoordinates = InNode->GetCornerCoordinates();

	if(Orientation == ESplitOrientation::ESO_Horizontal)
	{
		int32 FloorLength = CornerCoordinates.LowerRightY - CornerCoordinates.UpperLeftY;
		float PercentChanceOfSplit = (float)FloorLength / (float)FloorGridSizeY;
		PercentChanceOfSplit *= SplitChance;

		if(DiceRoll() > PercentChanceOfSplit)
		{
			return false;
		}

		if(FloorLength >= 2 * RoomMinY)
		{
			return true;
		}
		
	}
	else if(Orientation == ESplitOrientation::ESO_Vertical)
	{
		int32 FloorWidth = CornerCoordinates.LowerRightX - CornerCoordinates.UpperLeftX;
		float PercentChanceOfSplit = (float)FloorWidth / (float)FloorGridSizeX;
		PercentChanceOfSplit *= SplitChance;

		if(DiceRoll() > PercentChanceOfSplit)
		{
			return false;
		}
		
		if(FloorWidth >= 2 * RoomMinX)
		{
			return true;
		}
	}
	return false;
}

bool Floor::SplitAttempt(TSharedPtr<FloorNode> InNode)
{
	int32 HorizontalOrVertical = CoinFlip();
	if(HorizontalOrVertical == 0)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
	}
	else if (HorizontalOrVertical == 1)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
	}
	return false;
}

void Floor::SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	int32 SplitPointY = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftY + RoomMinY, InA->GetCornerCoordinates().LowerRightY - RoomMinY);

	InA->SetChildNodes(InB, InC);
	
	//New FloorNode top
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesB.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesB.LowerRightY = SplitPointY;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	FloorNodeStack.Push(InB);

	//New FloorNode bottom
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesC.UpperLeftY = SplitPointY;
	CornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	FloorNodeStack.Push(InC);
}

void Floor::SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	int32 SplitPointX = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftX + RoomMinX, InA->GetCornerCoordinates().LowerRightX - RoomMinX);

	InA->SetChildNodes(InB, InC);
	
	//New FloorNode left
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesB.LowerRightX = SplitPointX;
	CornerCoordinatesB.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	FloorNodeStack.Push(InB);

	//New FloorNode right
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.UpperLeftX = SplitPointX;
	CornerCoordinatesC.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	FloorNodeStack.Push(InC);
}

FCornerCoordinates Floor::ResizeRoom(FCornerCoordinates Coordinates, float ResizePercent)
{
	UE_LOG(LogTemp, Warning, TEXT("Resize : %f %"), ResizePercent);
	float ResizedWidth = ((float)Coordinates.LowerRightX - (float)Coordinates.UpperLeftX) * ResizePercent;
	float ResizedHeight = ((float)Coordinates.LowerRightY - (float)Coordinates.UpperLeftY) * ResizePercent;

	
	FCornerCoordinates ResizedCoordinates = Coordinates;
	ResizedCoordinates.UpperLeftX += ResizedWidth;
	ResizedCoordinates.UpperLeftY += ResizedHeight;
	ResizedCoordinates.LowerRightX -= ResizedWidth;
	ResizedCoordinates.LowerRightY -= ResizedHeight;
	return ResizedCoordinates;
}

void Floor::DrawFloorNodes(TObjectPtr<UWorld> World)
{	
	//Connect all nodes with hallways
	ConnectNodes(World, FindRoot(PartitionedFloor[0]));
	
	//Resize all rooms
	for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{
		FCornerCoordinates Coordinates = PartitionedFloor[i]->GetCornerCoordinates();

		float ResizePercent = FMath::RandRange(MinRoomSizePercent, 1.f);
		Coordinates = ResizeRoom(Coordinates, ResizePercent);
		PartitionedFloor[i]->SetCornerCoordinates(Coordinates);
		DrawFloorNode(World, Coordinates, FColor::Blue);
	}
	
	//Temporary separate loop to draw both the resized room as the original node
	/*for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{		
		FCornerCoordinates Coordinates = PartitionedFloor[i]->GetCornerCoordinates();
		DrawFloorNode(World, Coordinates, FColor::Blue);
	}*/
	//Temporary separate loop to draw parent nodes
	/*for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{
		if(PartitionedFloor[i]->GetParentNode() != nullptr)
		{
			FCornerCoordinates Coordinates = PartitionedFloor[i]->GetParentNode()->GetCornerCoordinates();
			DrawFloorNode(World, Coordinates, FColor::Red);				
		}
	}*/
}

void Floor::DrawFloorNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates, FColor DebugColor)
{
	const FVector UpperLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector UpperRight(Coordinates.LowerRightX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector LowerLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	const FVector LowerRight(Coordinates.LowerRightX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	
	DrawDebugLine(World, UpperLeft, UpperRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, UpperRight, LowerRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, LowerRight, LowerLeft, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, LowerLeft, UpperLeft, DebugColor, true, -1, 0, 20.f);	
}

/** Traverse Tree Recursively using Depth-first-search preorder **/
void Floor::ConnectNodes(TObjectPtr<UWorld> World, TSharedPtr<FloorNode> RootNode)
{
	if(RootNode == nullptr)
	{
		return;
	}
	if(RootNode->GetChildNodeA() != nullptr && RootNode->GetChildNodeB() != nullptr)
	{	
	    CreateHallway(World, RootNode->GetChildNodeA(), RootNode->GetChildNodeB());		
	}
	ConnectNodes(World, RootNode->GetChildNodeA());
	ConnectNodes(World, RootNode->GetChildNodeB());	
}

TSharedPtr<FloorNode> Floor::FindRoot(TSharedPtr<FloorNode> InNode)
{
	if(InNode->GetParentNode() != nullptr)
	{
		return FindRoot(InNode->GetParentNode());
	}
	return InNode;		
}

void Floor::CreateHallway(TObjectPtr<UWorld> World, TSharedPtr<FloorNode> NodeA, TSharedPtr<FloorNode> NodeB)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Hallway"));

	TSharedPtr<FloorNode> HallwayNode(new FloorNode());
	
	//TODO: Instead of using FCoordinates, create new FloorNode with the following coordinates
	FCornerCoordinates HallwayCornerCoordinates;
	
	//Get the center coordinates of Node A and B
	int32 NodeACenterX = (NodeA->GetCornerCoordinates().LowerRightX + NodeA->GetCornerCoordinates().UpperLeftX) / 2;
	int32 NodeACenterY = (NodeA->GetCornerCoordinates().LowerRightY + NodeA->GetCornerCoordinates().UpperLeftY) / 2;
	int32 NodeBCenterX = (NodeB->GetCornerCoordinates().LowerRightX + NodeB->GetCornerCoordinates().UpperLeftX) / 2;
	int32 NodeBCenterY = (NodeB->GetCornerCoordinates().LowerRightY + NodeB->GetCornerCoordinates().UpperLeftY) / 2;	
	
	int32 HallwayRadius = HallwayMinWidth / 2;

	//Shorter ternary version maybe not so useful, there might be more differentiation needed based on split orientation
	/*const bool IsSplitHorizontal = NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Horizontal;
	HallwayCornerCoordinates.UpperLeftX = (IsSplitHorizontal) ? NodeACenterX - HallwayRadius : NodeACenterX;
	HallwayCornerCoordinates.UpperLeftY = (IsSplitHorizontal) ? NodeACenterY : NodeACenterY - HallwayRadius;
	HallwayCornerCoordinates.LowerRightX = (IsSplitHorizontal) ? NodeBCenterX + HallwayRadius : NodeBCenterX;
	HallwayCornerCoordinates.LowerRightY = (IsSplitHorizontal) ? NodeBCenterY : NodeBCenterY + HallwayRadius;*/
	if(NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Horizontal)
	{
		HallwayCornerCoordinates.UpperLeftX = NodeACenterX - HallwayRadius;
		HallwayCornerCoordinates.UpperLeftY = NodeACenterY;
		HallwayCornerCoordinates.LowerRightX = NodeBCenterX + HallwayRadius;
		HallwayCornerCoordinates.LowerRightY = NodeBCenterY;
	}else
	{
		HallwayCornerCoordinates.UpperLeftX = NodeACenterX;
		HallwayCornerCoordinates.UpperLeftY = NodeACenterY - HallwayRadius;
		HallwayCornerCoordinates.LowerRightX = NodeBCenterX;
		HallwayCornerCoordinates.LowerRightY = NodeBCenterY + HallwayRadius;
	}
	
	HallwayNode->SetCornerCoordinates(HallwayCornerCoordinates);
	//Should Parent node also be the hallways parent? 
	NodeA->GetParentNode()->SetHallwayNode(HallwayNode);
	DrawFloorNode(World, HallwayCornerCoordinates, FColor::Black);
}

//Unused
void Floor::CalculateOverlap(int32 LineStartA, int32 LineEndA, int32 LineStartB, int32 LineEndB,
	int32 &OverlapStart, int32 &OverlapEnd)
{
	OverlapStart = TMathUtil<int32>::Min(LineStartA, LineStartB);
	OverlapEnd = TMathUtil<int32>::Max(LineEndA, LineEndB);
}



