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
	//UE_LOG(LogTemp, Warning, TEXT("Width: %f"), ResizedWidth);
	//UE_LOG(LogTemp, Warning, TEXT("Height: %f"), ResizedHeight);

	
	FCornerCoordinates ResizedCoordinates = Coordinates;
	ResizedCoordinates.UpperLeftX += ResizedWidth;
	ResizedCoordinates.UpperLeftY += ResizedHeight;
	ResizedCoordinates.LowerRightX -= ResizedWidth;
	ResizedCoordinates.LowerRightY -= ResizedHeight;
	return ResizedCoordinates;
}

void Floor::DrawFloorNodes(TObjectPtr<UWorld> World)
{
	for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{
		FCornerCoordinates Coordinates = PartitionedFloor[i]->GetCornerCoordinates();
		//DrawFloorNode(World, Coordinates, FColor::Green);

		float ResizePercent = FMath::RandRange(MinRoomSizePercent, 1.f);
		PartitionedFloor[i]->SetCornerCoordinates(ResizeRoom(Coordinates, ResizePercent));
	}
	//Temporary separate loop to draw both the resized room as the original node
	for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{		
		FCornerCoordinates Coordinates = PartitionedFloor[i]->GetCornerCoordinates();
		DrawFloorNode(World, Coordinates, FColor::Blue);
	}
	//Temporary separate loop to draw parent nodes
	for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{
		if(PartitionedFloor[i]->GetParentNode() != nullptr)
		{
			FCornerCoordinates Coordinates = PartitionedFloor[i]->GetParentNode()->GetCornerCoordinates();
			DrawFloorNode(World, Coordinates, FColor::Red);				
		}
	}
	//Temporary separate loop to draw hallways between nodes
	for (int32 i = 0; i < PartitionedFloor.Num(); i+=2)
	{
		CreateHallways(World, PartitionedFloor[i]->GetParentNode());
	}
}

void Floor::DrawFloorNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates, FColor DebugColor)
{
	const FVector UpperLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector UpperRight(Coordinates.LowerRightX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector LowerLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	const FVector LowerRight(Coordinates.LowerRightX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	
	DrawDebugLine(World, UpperLeft, UpperRight, DebugColor, true, -1, 0, 5.f);
	DrawDebugLine(World, UpperRight, LowerRight, DebugColor, true, -1, 0, 5.f);
	DrawDebugLine(World, LowerRight, LowerLeft, DebugColor, true, -1, 0, 5.f);
	DrawDebugLine(World, LowerLeft, UpperLeft, DebugColor, true, -1, 0, 5.f);	
}

void Floor::CreateHallways(TObjectPtr<UWorld> World, TSharedPtr<FloorNode> NodeToConnect)
{

	TSharedPtr<FloorNode> NodeA = NodeToConnect->GetChildNodeA();
	TSharedPtr<FloorNode> NodeB = NodeToConnect->GetChildNodeB();

	if(NodeA->GetChildNodeA() != nullptr && NodeA->GetChildNodeB() != nullptr)
	{		
		CreateHallways(World, NodeA);
	}
	else
	{
		NodesASide.Add(NodeA);
	}

	if(NodeB->GetChildNodeA() != nullptr && NodeB->GetChildNodeB() != nullptr)
	{		
		CreateHallways(World, NodeB);
	}
	else
	{
		NodesBSide.Add(NodeB);
	}


	/*if(NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Horizontal)
	{
		float NodeALeftX = NodeA->GetCornerCoordinates().UpperLeftX;
		float NodeARightX = NodeA->GetCornerCoordinates().LowerRightX;
		float NodeBLeftX = NodeB->GetCornerCoordinates().UpperLeftX;
		float NodeBRightX = NodeB->GetCornerCoordinates().LowerRightX; 
		if(NodeALeftX >= NodeBLeftX)
		{
			//Scenario 1.
			if(NodeARightX <= NodeBRightX)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Green, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Green, true, -1, 0, 5.f);				
			}
			//Scenario 4.
			else if(NodeARightX > NodeBRightX)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Yellow, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Yellow, true, -1, 0, 5.f);
			}
		}
		else if(NodeALeftX < NodeBLeftX)
		{
			//Scenario 2.
			if(NodeARightX >= NodeBRightX)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Red, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Red, true, -1, 0, 5.f);				
			}
			//Scenario 3.
			else if(NodeARightX < NodeBRightX)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Blue, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Blue, true, -1, 0, 5.f);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Horizontal"));
	}
	else if(NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Vertical)
	{
		float NodeAUpperY = NodeA->GetCornerCoordinates().UpperLeftY;
		float NodeALowerY = NodeA->GetCornerCoordinates().LowerRightY;
		float NodeBUpperY = NodeB->GetCornerCoordinates().UpperLeftY;
		float NodeBLowerY = NodeB->GetCornerCoordinates().LowerRightY; 
		if(NodeAUpperY >= NodeBUpperY)
		{
			//Scenario 1.
			if(NodeALowerY <= NodeBLowerY)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Green, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Green, true, -1, 0, 5.f);				
			}
			//Scenario 4.
			else if(NodeALowerY > NodeBLowerY)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Yellow, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Yellow, true, -1, 0, 5.f);
			}
		}
		else if(NodeAUpperY < NodeBUpperY)
		{
			//Scenario 2.
			if(NodeALowerY >= NodeBLowerY)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Red, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Red, true, -1, 0, 5.f);				
			}
			//Scenario 3.
			else if(NodeALowerY < NodeBLowerY)
			{
				const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
				const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
				DrawDebugLine(World, UpperLeft, UpperRight, FColor::Blue, true, -1, 0, 5.f);				
				DrawDebugLine(World, LowerLeft, LowerRight, FColor::Blue, true, -1, 0, 5.f);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Vertical"));
		
	}*/

	if(NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Horizontal)
	{
		int32 OverlapStart;
		int32 OverlapEnd;
		
		CalculateOverlap(NodeA->GetCornerCoordinates().UpperLeftX, NodeA->GetCornerCoordinates().LowerRightX,
						 NodeB->GetCornerCoordinates().UpperLeftX, NodeB->GetCornerCoordinates().LowerRightX, OverlapStart, OverlapEnd);

		//Do not create hallway if there's no overlap
		if(OverlapEnd > OverlapStart)
		{
			return;
		}

		const FVector UpperRight(OverlapStart * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
		const FVector UpperLeft(OverlapStart * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
		const FVector LowerLeft(OverlapEnd * UnitLength, NodeB->GetCornerCoordinates().LowerRightY * UnitLength, 0.f);
		const FVector LowerRight(OverlapEnd * UnitLength, NodeA->GetCornerCoordinates().UpperLeftY * UnitLength, 0.f);
		DrawDebugLine(World, UpperLeft, UpperRight, FColor::Green, true, -1, 0, 5.f);				
		DrawDebugLine(World, LowerLeft, LowerRight, FColor::Green, true, -1, 0, 5.f);
	}
	else if(NodeA->GetSplitOrientation() == ESplitOrientation::ESO_Vertical)
	{
		int32 OverlapStart;
		int32 OverlapEnd;
		
		CalculateOverlap(NodeA->GetCornerCoordinates().UpperLeftY, NodeA->GetCornerCoordinates().LowerRightY,
		                 NodeB->GetCornerCoordinates().UpperLeftY, NodeB->GetCornerCoordinates().LowerRightY, OverlapStart, OverlapEnd);

		//Do not create hallway if there's no overlap
		if(OverlapEnd > OverlapStart)
		{
			return;
		}

		const FVector UpperRight(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, OverlapStart * UnitLength, 0.f);
		const FVector UpperLeft(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, OverlapStart * UnitLength, 0.f);
		const FVector LowerLeft(NodeB->GetCornerCoordinates().LowerRightX * UnitLength, OverlapEnd * UnitLength, 0.f);
		const FVector LowerRight(NodeA->GetCornerCoordinates().UpperLeftX * UnitLength, OverlapEnd * UnitLength, 0.f);
		DrawDebugLine(World, UpperLeft, UpperRight, FColor::Green, true, -1, 0, 5.f);				
		DrawDebugLine(World, LowerLeft, LowerRight, FColor::Green, true, -1, 0, 5.f);
	}

	UE_LOG(LogTemp, Warning, TEXT("%i"), NodesASide.Num());
	UE_LOG(LogTemp, Warning, TEXT("%i"), NodesBSide.Num());
	NodesASide.Empty();
	NodesBSide.Empty();
	//Go up the tree recursively
	/*TSharedPtr<FloorNode> ParentNode = NodeToConnect->GetParentNode();
	if(ParentNode == nullptr) return;
	ConnectFloorNodes(World, ParentNode);*/
}

void Floor::CalculateOverlap(int32 LineStartA, int32 LineEndA, int32 LineStartB, int32 LineEndB,
	int32 &OverlapStart, int32 &OverlapEnd)
{
	OverlapStart = TMathUtil<int32>::Min(LineStartA, LineStartB);
	OverlapEnd = TMathUtil<int32>::Max(LineEndA, LineEndB);
}




