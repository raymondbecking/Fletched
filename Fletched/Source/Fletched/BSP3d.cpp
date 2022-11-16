#include "BSP3d.h"
#include "BSP3dNode.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"

BSP3d::BSP3d()
{
	BSPGridSizeX = 200;
	BSPGridSizeY = 200;
	BSPGridSizeZ = 200;
	RoomMinX = 20;
	RoomMinY = 20;
	RoomMinZ = 20;

	UnitLength = 100.f;

	SplitChance = 1.3f;

	MinRoomSizePercent = 0.4f;
	MaxRoomSizePercent = 0.9f;

	HallwayMinWidth = 2;

	MaxConnectAttempts = 20;

	RandomHallwayChance = .5f;

	UE_LOG(LogTemp, Warning, TEXT("BSP3d created."));

}

BSP3d::~BSP3d()
{
	UE_LOG(LogTemp, Warning, TEXT("BSP3d destroyed."));
}

void BSP3d::Partition()
{
	FCornerCoordinates CornerCoordinatesA = {0,0, 0,
		BSPGridSizeX, BSPGridSizeY, BSPGridSizeZ};
	NodeStack.Push(TSharedPtr<BSP3dNode>(new BSP3dNode(CornerCoordinatesA)));

	while (NodeStack.Num() > 0)
	{
		TSharedPtr<BSP3dNode> A = NodeStack.Pop();

		bool bNodeWasSplit = SplitAttempt(A);
		if(!bNodeWasSplit)
		{
			PartitionedNodes.Push(A);
		}		
	}
}

int32 BSP3d::CoinFlip()
{
	return FMath::RandRange(0,2);
}

float BSP3d::DiceRoll()
{
	return FMath::FRandRange(0.f, 1.f);
}

bool BSP3d::ShouldSplitNode(TSharedPtr<BSP3dNode> InNode, ESplitOrientation Orientation)
{
	FCornerCoordinates CornerCoordinates = InNode->GetCornerCoordinates();

	if(Orientation == ESplitOrientation::ESO_Horizontal)
	{
		int32 NodeLength = CornerCoordinates.FrontLowerRightY - CornerCoordinates.BackUpperLeftY;
		float PercentChanceOfSplit = (float)NodeLength / (float)BSPGridSizeY;
		PercentChanceOfSplit *= SplitChance;

		
		if(DiceRoll() > PercentChanceOfSplit)
		{
			return false;
		}

		if(NodeLength >= 2 * RoomMinY)
		{
			return true;
		}
		
	}
	else if(Orientation == ESplitOrientation::ESO_Vertical)
	{
		int32 NodeWidth = CornerCoordinates.FrontLowerRightX - CornerCoordinates.BackUpperLeftX;
		float PercentChanceOfSplit = (float)NodeWidth / (float)BSPGridSizeX;
		PercentChanceOfSplit *= SplitChance;

		if(DiceRoll() > PercentChanceOfSplit)
		{
			return false;
		}
		
		if(NodeWidth >= 2 * RoomMinX)
		{
			return true;
		}
	}
	else if(Orientation == ESplitOrientation::ESO_Depth)
	{
		int32 NodeDepth = CornerCoordinates.FrontLowerRightZ - CornerCoordinates.BackUpperLeftZ;
		float PercentChanceOfSplit = (float)NodeDepth / (float)BSPGridSizeZ;
		PercentChanceOfSplit *= SplitChance;

		if(DiceRoll() > PercentChanceOfSplit)
		{
			return false;
		}
		
		if(NodeDepth >= 2 * RoomMinZ)
		{
			return true;
		}
	}
	return false;
}

bool BSP3d::SplitAttempt(TSharedPtr<BSP3dNode> InNode)
{
	int32 HorizontalOrVerticalOrDepth = CoinFlip();
	if(HorizontalOrVerticalOrDepth == 0)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Depth))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitDepth(InNode, B, C);
			return true;
		}
	}
	else if (HorizontalOrVerticalOrDepth == 1)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Depth))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitDepth(InNode, B, C);
			return true;
		}
	}
	else if (HorizontalOrVerticalOrDepth == 2)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Depth))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitDepth(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<BSP3dNode> B(new BSP3dNode());
			TSharedPtr<BSP3dNode> C(new BSP3dNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
	}
	return false;
}

void BSP3d::SplitHorizontal(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC)
{
	int32 SplitPointY = FMath::RandRange(InA->GetCornerCoordinates().BackUpperLeftY + RoomMinY, InA->GetCornerCoordinates().FrontLowerRightY - RoomMinY);

	InA->SetChildNodes(InB, InC);
	
	//New BSP3dNode top
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.BackUpperLeftX = InA->GetCornerCoordinates().BackUpperLeftX;
	CornerCoordinatesB.BackUpperLeftY = InA->GetCornerCoordinates().BackUpperLeftY;
	CornerCoordinatesB.BackUpperLeftZ = InA->GetCornerCoordinates().BackUpperLeftZ;
	CornerCoordinatesB.FrontLowerRightX = InA->GetCornerCoordinates().FrontLowerRightX;
	CornerCoordinatesB.FrontLowerRightY = SplitPointY;
	CornerCoordinatesB.FrontLowerRightZ = InA->GetCornerCoordinates().FrontLowerRightZ;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	NodeStack.Push(InB);

	//New BSP3dNode bottom
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.BackUpperLeftX = InA->GetCornerCoordinates().BackUpperLeftX;
	CornerCoordinatesC.BackUpperLeftY = SplitPointY;
	CornerCoordinatesC.BackUpperLeftZ = InA->GetCornerCoordinates().BackUpperLeftZ;
	CornerCoordinatesC.FrontLowerRightX = InA->GetCornerCoordinates().FrontLowerRightX;
	CornerCoordinatesC.FrontLowerRightY = InA->GetCornerCoordinates().FrontLowerRightY;
	CornerCoordinatesC.FrontLowerRightZ = InA->GetCornerCoordinates().FrontLowerRightZ;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	NodeStack.Push(InC);
}

void BSP3d::SplitVertical(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC)
{
	int32 SplitPointX = FMath::RandRange(InA->GetCornerCoordinates().BackUpperLeftX + RoomMinX, InA->GetCornerCoordinates().FrontLowerRightX - RoomMinX);

	InA->SetChildNodes(InB, InC);
	
	//New BSP3dNode left
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.BackUpperLeftX = InA->GetCornerCoordinates().BackUpperLeftX;
	CornerCoordinatesB.BackUpperLeftY = InA->GetCornerCoordinates().BackUpperLeftY;
	CornerCoordinatesB.BackUpperLeftZ = InA->GetCornerCoordinates().BackUpperLeftZ;
	CornerCoordinatesB.FrontLowerRightX = SplitPointX;
	CornerCoordinatesB.FrontLowerRightY = InA->GetCornerCoordinates().FrontLowerRightY;
	CornerCoordinatesB.FrontLowerRightZ = InA->GetCornerCoordinates().FrontLowerRightZ;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	NodeStack.Push(InB);

	//New BSP3dNode right
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.BackUpperLeftX = SplitPointX;
	CornerCoordinatesC.BackUpperLeftY = InA->GetCornerCoordinates().BackUpperLeftY;
	CornerCoordinatesC.BackUpperLeftZ = InA->GetCornerCoordinates().BackUpperLeftZ;
	CornerCoordinatesC.FrontLowerRightX = InA->GetCornerCoordinates().FrontLowerRightX;
	CornerCoordinatesC.FrontLowerRightY = InA->GetCornerCoordinates().FrontLowerRightY;
	CornerCoordinatesC.FrontLowerRightZ = InA->GetCornerCoordinates().FrontLowerRightZ;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	NodeStack.Push(InC);
}

void BSP3d::SplitDepth(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC)
{
	int32 SplitPointZ = FMath::RandRange(InA->GetCornerCoordinates().BackUpperLeftZ + RoomMinZ, InA->GetCornerCoordinates().FrontLowerRightZ - RoomMinZ);

	InA->SetChildNodes(InB, InC);
	
	//New BSP3dNode left
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.BackUpperLeftX = InA->GetCornerCoordinates().BackUpperLeftX;
	CornerCoordinatesB.BackUpperLeftY = InA->GetCornerCoordinates().BackUpperLeftY;
	CornerCoordinatesB.BackUpperLeftZ = InA->GetCornerCoordinates().BackUpperLeftZ;
	CornerCoordinatesB.FrontLowerRightX = InA->GetCornerCoordinates().FrontLowerRightX;
	CornerCoordinatesB.FrontLowerRightY = InA->GetCornerCoordinates().FrontLowerRightY;
	CornerCoordinatesB.FrontLowerRightZ = SplitPointZ;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Depth);
	NodeStack.Push(InB);

	//New BSP3dNode right
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.BackUpperLeftX = InA->GetCornerCoordinates().BackUpperLeftX;
	CornerCoordinatesC.BackUpperLeftY = InA->GetCornerCoordinates().BackUpperLeftY;
	CornerCoordinatesC.BackUpperLeftZ = SplitPointZ;
	CornerCoordinatesC.FrontLowerRightX = InA->GetCornerCoordinates().FrontLowerRightX;
	CornerCoordinatesC.FrontLowerRightY = InA->GetCornerCoordinates().FrontLowerRightY;
	CornerCoordinatesC.FrontLowerRightZ = InA->GetCornerCoordinates().FrontLowerRightZ;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Depth);
	NodeStack.Push(InC);
}

FCornerCoordinates BSP3d::ResizeRoom(FCornerCoordinates Coordinates, float ResizePercent)
{
	//Calculate new length of each side of the square
	int32 ResizedWidth = (Coordinates.FrontLowerRightX - Coordinates.BackUpperLeftX) * ResizePercent;
	int32 ResizedHeight = (Coordinates.FrontLowerRightY - Coordinates.BackUpperLeftY) * ResizePercent;

	//Calculate how much to move each corner
	int32 ResizedXCoordinate = (Coordinates.FrontLowerRightX - Coordinates.BackUpperLeftX - ResizedWidth) / 2;
	int32 ResizedYCoordinate = (Coordinates.FrontLowerRightY - Coordinates.BackUpperLeftY - ResizedHeight) / 2;
	
	FCornerCoordinates ResizedCoordinates = Coordinates;
	ResizedCoordinates.BackUpperLeftX += ResizedXCoordinate;
	ResizedCoordinates.BackUpperLeftY += ResizedYCoordinate;
	ResizedCoordinates.FrontLowerRightX -= ResizedXCoordinate;
	ResizedCoordinates.FrontLowerRightY -= ResizedYCoordinate;
	return ResizedCoordinates;
}

void BSP3d::DrawBSPNodes(TObjectPtr<UWorld> World)
{		
	//Resize and draw all rooms
	for (int32 i = 0; i < PartitionedNodes.Num(); i++)
	{
		FCornerCoordinates Coordinates = PartitionedNodes[i]->GetCornerCoordinates();

		//float ResizePercent = FMath::RandRange(MinRoomSizePercent, MaxRoomSizePercent);
		//Coordinates = ResizeRoom(Coordinates, ResizePercent);
		//PartitionedNodes[i]->SetCornerCoordinates(Coordinates);
		DrawBSPNode(World, Coordinates, FColor::Blue);
	}
	//Connect all nodes with hallways
	//ConnectNodes(World, FindRootNode(PartitionedNodes[0]));
}

void BSP3d::DrawBSPNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates, FColor DebugColor)
{
	const FVector BottomUpperLeft(Coordinates.BackUpperLeftX * UnitLength, Coordinates.BackUpperLeftY * UnitLength, Coordinates.FrontLowerRightZ * UnitLength);
	const FVector BottomUpperRight(Coordinates.FrontLowerRightX * UnitLength, Coordinates.BackUpperLeftY * UnitLength, Coordinates.FrontLowerRightZ * UnitLength);
	const FVector BottomLowerLeft(Coordinates.BackUpperLeftX * UnitLength, Coordinates.FrontLowerRightY * UnitLength, Coordinates.FrontLowerRightZ * UnitLength);
	const FVector BottomLowerRight(Coordinates.FrontLowerRightX * UnitLength, Coordinates.FrontLowerRightY * UnitLength, Coordinates.FrontLowerRightZ * UnitLength);
	
	const FVector TopUpperLeft(Coordinates.BackUpperLeftX * UnitLength, Coordinates.BackUpperLeftY * UnitLength, Coordinates.BackUpperLeftZ * UnitLength);
	const FVector TopUpperRight(Coordinates.FrontLowerRightX * UnitLength, Coordinates.BackUpperLeftY * UnitLength, Coordinates.BackUpperLeftZ * UnitLength);
	const FVector TopLowerLeft(Coordinates.BackUpperLeftX * UnitLength, Coordinates.FrontLowerRightY * UnitLength, Coordinates.BackUpperLeftZ * UnitLength);
	const FVector TopLowerRight(Coordinates.FrontLowerRightX * UnitLength, Coordinates.FrontLowerRightY * UnitLength, Coordinates.BackUpperLeftZ * UnitLength);

	DrawDebugLine(World, BottomUpperLeft, BottomUpperRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomUpperRight, BottomLowerRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomLowerRight, BottomLowerLeft, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomLowerLeft, BottomUpperLeft, DebugColor, true, -1, 0, 20.f);
	
	DrawDebugLine(World, TopUpperLeft, TopUpperRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, TopUpperRight, TopLowerRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, TopLowerRight, TopLowerLeft, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, TopLowerLeft, TopUpperLeft, DebugColor, true, -1, 0, 20.f);	
	
	DrawDebugLine(World, BottomUpperLeft, TopUpperLeft, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomUpperRight, TopUpperRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomLowerRight, TopLowerRight, DebugColor, true, -1, 0, 20.f);
	DrawDebugLine(World, BottomLowerLeft, TopLowerLeft, DebugColor, true, -1, 0, 20.f);
}

//TODO: Change to support Z coordinate
/** Traverse Tree Recursively using Depth-first-search preorder to connect all nodes in the tree **/
void BSP3d::ConnectNodes(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> RootNode)
{
	if (RootNode == nullptr)
	{
		return;
	}
	
	if (RootNode->GetChildNodeA() != nullptr && RootNode->GetChildNodeB() != nullptr)
	{
		int32 AttemptsLeft = MaxConnectAttempts;
		bool bAttemptSuccessful;

		//More attempts are needed the lower MinRoomSizePercent is set, extra attempts occur 1 in 6 generations of the entire dungeon at 0.5f 
		do
		{
			AttemptsLeft--;
			bAttemptSuccessful = ConnectAttempt(World, RootNode->GetChildNodeA(), RootNode->GetChildNodeB(),
					   RootNode->GetChildNodeA()->GetSplitOrientation());
			if(AttemptsLeft == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("All connect attempts failed!"));
			}
		}
		while(bAttemptSuccessful == false && AttemptsLeft > 0);

		//Randomly add hallways, End nodes don't need more hallways
		if (RootNode->GetChildNodeA()->GetChildNodeA() != nullptr &&
			RootNode->GetChildNodeB()->GetChildNodeA() != nullptr)
		{
			if (DiceRoll() < RandomHallwayChance)
			{
				// Doesn't matter if attempt is successful or not since its an extra hallway, attempts rarely fail anyways
				ConnectAttempt(World, RootNode->GetChildNodeA(), RootNode->GetChildNodeB(),
				               RootNode->GetChildNodeA()->GetSplitOrientation());
			}
		}
	}	
	ConnectNodes(World, RootNode->GetChildNodeA());
	ConnectNodes(World, RootNode->GetChildNodeB());	
}

//TODO: Change to support Z coordinate
/** Finds the best nodes to connect from both the NodeA side and NodeB side and calls CreateHallway on success **/
bool BSP3d::ConnectAttempt(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB, ESplitOrientation ConnectOrientation)
{
	if (NodeA == nullptr || NodeB == nullptr)
	{
		return true;
	}

	if (NodeA->GetChildNodeA() != nullptr && NodeA->GetChildNodeB() != nullptr)
	{
		bool bChildACloser = DistanceBetweenNodes(NodeA->GetChildNodeA(), NodeB) < DistanceBetweenNodes(NodeA->GetChildNodeB(), NodeB);
		if (NodeA->GetChildNodeA() != nullptr)
		{
			//Coinflip when it does not matter which node to choose, adds randomness for repeated connect attempts
			bChildACloser = (ConnectOrientation == NodeA->GetChildNodeA()->GetSplitOrientation()) ? bChildACloser : UKismetMathLibrary::Conv_IntToBool(CoinFlip());
		}
		if (bChildACloser) //Keep going deeper in the closest node of NodeA tree to find which node to connect
		{
			return ConnectAttempt(World, NodeA->GetChildNodeA(), NodeB, ConnectOrientation);
		}
		return ConnectAttempt(World, NodeA->GetChildNodeB(), NodeB, ConnectOrientation);
	}

	if (NodeB->GetChildNodeA() != nullptr && NodeB->GetChildNodeB() != nullptr)
	{
		bool bIsChildACloser = DistanceBetweenNodes(NodeA, NodeB->GetChildNodeA()) < DistanceBetweenNodes(NodeA, NodeB->GetChildNodeB());
		if (NodeA->GetChildNodeA() != nullptr)
		{
			//Coinflip when it does not matter which node to choose, adds randomness for repeated connect attempts
			bIsChildACloser = (ConnectOrientation == NodeA->GetChildNodeA()->GetSplitOrientation()) ? bIsChildACloser : UKismetMathLibrary::Conv_IntToBool(CoinFlip());
		}
		if (bIsChildACloser) //Keep going deeper in the closest node of NodeB tree to find which node to connect
		{
			return ConnectAttempt(World, NodeA, NodeB->GetChildNodeA(), ConnectOrientation);
		}
		return ConnectAttempt(World, NodeA, NodeB->GetChildNodeB(), ConnectOrientation);
	}	

	int32 OverlapStart;
	int32 OverlapEnd;
	bool bHasOverlap;
	
	if (ConnectOrientation == ESplitOrientation::ESO_Horizontal)
	{
		bHasOverlap = CalculateHasOverlap(NodeA->GetCornerCoordinates().BackUpperLeftX,
										 NodeA->GetCornerCoordinates().FrontLowerRightX,
										 NodeB->GetCornerCoordinates().BackUpperLeftX,
										 NodeB->GetCornerCoordinates().FrontLowerRightX, OverlapStart, OverlapEnd);
	}
	else// if (ConnectOrientation == ESplitOrientation::ESO_Vertical)
		{
		bHasOverlap = CalculateHasOverlap(NodeA->GetCornerCoordinates().BackUpperLeftY,
										 NodeA->GetCornerCoordinates().FrontLowerRightY,
										 NodeB->GetCornerCoordinates().BackUpperLeftY,
										 NodeB->GetCornerCoordinates().FrontLowerRightY, OverlapStart, OverlapEnd);
		}
	if (bHasOverlap)
	{
		//Connect the matching nodes
		CreateHallway(World, NodeA, NodeB, OverlapStart, OverlapEnd);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connect Attempt Failed"));
	return false;	
}

//TODO: Change to support Z coordinate
void BSP3d::CreateHallway(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB, int32 OverlapStart, int32 OverlapEnd)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Hallway"));

	TSharedPtr<BSP3dNode> HallwayNode(new BSP3dNode());
	FCornerCoordinates HallwayCornerCoordinates;
	
	int32 HallwayRadius = HallwayMinWidth / 2;
	int32 HallwayRandomPos = FMath::RandRange(OverlapStart + HallwayRadius,OverlapEnd - HallwayRadius);
	FColor HallwayColor = FColor::Black;

	if (NodeB->GetCornerCoordinates().BackUpperLeftY > NodeA->GetCornerCoordinates().FrontLowerRightY)
	{
		HallwayCornerCoordinates.BackUpperLeftX = HallwayRandomPos - HallwayRadius;
		HallwayCornerCoordinates.BackUpperLeftY = NodeA->GetCornerCoordinates().FrontLowerRightY;
		HallwayCornerCoordinates.FrontLowerRightX = HallwayRandomPos + HallwayRadius;
		HallwayCornerCoordinates.FrontLowerRightY = NodeB->GetCornerCoordinates().BackUpperLeftY;
		HallwayColor = FColor::Green;		
	}
	else// if(NodeB->GetCornerCoordinates().BackUpperLeftX > NodeA->GetCornerCoordinates().FrontLowerRightX)
	{
		HallwayCornerCoordinates.BackUpperLeftX = NodeB->GetCornerCoordinates().BackUpperLeftX;
		HallwayCornerCoordinates.BackUpperLeftY = HallwayRandomPos - HallwayRadius;
		HallwayCornerCoordinates.FrontLowerRightX = NodeA->GetCornerCoordinates().FrontLowerRightX;
		HallwayCornerCoordinates.FrontLowerRightY = HallwayRandomPos + HallwayRadius;
		HallwayColor = FColor::Red;
	}	
	
	HallwayNode->SetCornerCoordinates(HallwayCornerCoordinates);
	//Should Parent node also be the hallways parent? 
	NodeA->GetParentNode()->SetHallwayNode(HallwayNode);
	DrawBSPNode(World, HallwayCornerCoordinates, HallwayColor);
}

TSharedPtr<BSP3dNode> BSP3d::FindRootNode(TSharedPtr<BSP3dNode> InNode)
{
	if (InNode->GetParentNode() != nullptr)
	{
		return FindRootNode(InNode->GetParentNode());
	}
	return InNode;
}

//TODO: Change to support square surface instead of line
/** Not actual overlap is calculated, but rather what part of 2 sides of different nodes align **/
bool BSP3d::CalculateHasOverlap(int32 LineStartA, int32 LineEndA, int32 LineStartB, int32 LineEndB,
                                int32& OverlapStart, int32& OverlapEnd)
{
	OverlapStart = TMathUtil<int32>::Max(LineStartA, LineStartB);
	OverlapEnd = TMathUtil<int32>::Min(LineEndA, LineEndB);
	return OverlapEnd - OverlapStart > 0;
}

/** Euclidean distance between the centers of 2 nodes **/
int32 BSP3d::DistanceBetweenNodes(TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB)
{
	if (NodeA == nullptr || NodeB == nullptr)
	{
		return 0;
	}
	float NodeACenterX = (NodeA->GetCornerCoordinates().FrontLowerRightX + NodeA->GetCornerCoordinates().BackUpperLeftX) / 2;
	float NodeACenterY = (NodeA->GetCornerCoordinates().FrontLowerRightY + NodeA->GetCornerCoordinates().BackUpperLeftY) / 2;
	FVector2f NodeACenter = FVector2f(NodeACenterX, NodeACenterY);

	float NodeBCenterX = (NodeB->GetCornerCoordinates().FrontLowerRightX + NodeB->GetCornerCoordinates().BackUpperLeftX) / 2;
	float NodeBCenterY = (NodeB->GetCornerCoordinates().FrontLowerRightY + NodeB->GetCornerCoordinates().BackUpperLeftY) / 2;
	FVector2f NodeBCenter = FVector2f(NodeBCenterX, NodeBCenterY);
	
	return FVector2f::Distance(NodeACenter, NodeBCenter);
}




