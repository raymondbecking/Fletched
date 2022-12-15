#include "BSP.h"
#include "BSPNode.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "MathUtil.h"
#include "Kismet/KismetMathLibrary.h"

BSP::BSP()
{
	BSPGridSizeX = 200;
	BSPGridSizeY = 200;
	RoomMinX = 20;
	RoomMinY = 20;

	UnitLength = 100.f;

	SplitChance = 1.3f;

	MinRoomSizePercent = 0.4f;
	MaxRoomSizePercent = 0.9f;

	HallwayMinWidth = 2;

	MaxConnectAttempts = 20;

	RandomHallwayChance = .5f;

	UE_LOG(LogTemp, Warning, TEXT("BSP created."));

}

BSP::~BSP()
{
	UE_LOG(LogTemp, Warning, TEXT("BSP destroyed."));
}

void BSP::Partition()
{
	FCornerCoordinates CornerCoordinatesA = {0,0, BSPGridSizeX, BSPGridSizeY};
	BSPNodeStack.Push(TSharedPtr<BSPNode>(new BSPNode(CornerCoordinatesA)));

	while (BSPNodeStack.Num() > 0)
	{
		TSharedPtr<BSPNode> A = BSPNodeStack.Pop();

		bool bNodeWasSplit = SplitAttempt(A);
		if(!bNodeWasSplit)
		{
			PartitionedBSPNodes.Push(A);
		}		
	}
}

int32 BSP::CoinFlip()
{
	return FMath::RandRange(0,1);
}

float BSP::DiceRoll()
{
	return FMath::FRandRange(0.f, 1.f);
}

bool BSP::ShouldSplitNode(TSharedPtr<BSPNode> InNode, ESplitOrientation Orientation)
{
	FCornerCoordinates CornerCoordinates = InNode->GetCornerCoordinates();

	if(Orientation == ESplitOrientation::ESO_Horizontal)
	{
		int32 NodeLength = CornerCoordinates.LowerRightY - CornerCoordinates.UpperLeftY;
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
		int32 NodeWidth = CornerCoordinates.LowerRightX - CornerCoordinates.UpperLeftX;
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
	return false;
}

bool BSP::SplitAttempt(TSharedPtr<BSPNode> InNode)
{
	int32 HorizontalOrVertical = CoinFlip();
	if(HorizontalOrVertical == 0)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<BSPNode> B(new BSPNode());
			TSharedPtr<BSPNode> C(new BSPNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<BSPNode> B(new BSPNode());
			TSharedPtr<BSPNode> C(new BSPNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
	}
	else if (HorizontalOrVertical == 1)
	{
		if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			//Try to split node Vertically
			TSharedPtr<BSPNode> B(new BSPNode());
			TSharedPtr<BSPNode> C(new BSPNode());
			
			SplitVertical(InNode, B, C);
			return true;
		}
		else if(ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			//Try to split node Horizontally
			TSharedPtr<BSPNode> B(new BSPNode());
			TSharedPtr<BSPNode> C(new BSPNode());

			SplitHorizontal(InNode, B, C);
			return true;
		}
	}
	return false;
}

void BSP::SplitHorizontal(TSharedPtr<BSPNode> InA, TSharedPtr<BSPNode> InB, TSharedPtr<BSPNode> InC)
{
	int32 SplitPointY = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftY + RoomMinY, InA->GetCornerCoordinates().LowerRightY - RoomMinY);

	InA->SetChildNodes(InB, InC);
	
	//New BSPNode top
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesB.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesB.LowerRightY = SplitPointY;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	BSPNodeStack.Push(InB);

	//New BSPNode bottom
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesC.UpperLeftY = SplitPointY;
	CornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Horizontal);
	BSPNodeStack.Push(InC);
}

void BSP::SplitVertical(TSharedPtr<BSPNode> InA, TSharedPtr<BSPNode> InB, TSharedPtr<BSPNode> InC)
{
	int32 SplitPointX = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftX + RoomMinX, InA->GetCornerCoordinates().LowerRightX - RoomMinX);

	InA->SetChildNodes(InB, InC);
	
	//New BSPNode left
	FCornerCoordinates CornerCoordinatesB;
	CornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	CornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesB.LowerRightX = SplitPointX;
	CornerCoordinatesB.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InB->SetCornerCoordinates(CornerCoordinatesB);
	InB->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	BSPNodeStack.Push(InB);

	//New BSPNode right
	FCornerCoordinates CornerCoordinatesC;
	CornerCoordinatesC.UpperLeftX = SplitPointX;
	CornerCoordinatesC.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	CornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	CornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	InC->SetCornerCoordinates(CornerCoordinatesC);
	InC->SetParentNode(InA);
	InB->SetSplitOrientation(ESplitOrientation::ESO_Vertical);
	BSPNodeStack.Push(InC);
}

FCornerCoordinates BSP::ResizeRoom(FCornerCoordinates Coordinates, float ResizePercent)
{
	//Calculate new length of each side of the square
	int32 ResizedWidth = (Coordinates.LowerRightX - Coordinates.UpperLeftX) * ResizePercent;
	int32 ResizedHeight = (Coordinates.LowerRightY - Coordinates.UpperLeftY) * ResizePercent;

	//Calculate how much to move each corner
	int32 ResizedXCoordinate = (Coordinates.LowerRightX - Coordinates.UpperLeftX - ResizedWidth) / 2;
	int32 ResizedYCoordinate = (Coordinates.LowerRightY - Coordinates.UpperLeftY - ResizedHeight) / 2;
	
	FCornerCoordinates ResizedCoordinates = Coordinates;
	ResizedCoordinates.UpperLeftX += ResizedXCoordinate;
	ResizedCoordinates.UpperLeftY += ResizedYCoordinate;
	ResizedCoordinates.LowerRightX -= ResizedXCoordinate;
	ResizedCoordinates.LowerRightY -= ResizedYCoordinate;
	return ResizedCoordinates;
}

void BSP::DrawBSPNodes(TObjectPtr<UWorld> World)
{		
	//Resize and draw all rooms
	for (int32 i = 0; i < PartitionedBSPNodes.Num(); i++)
	{
		FCornerCoordinates Coordinates = PartitionedBSPNodes[i]->GetCornerCoordinates();

		float ResizePercent = FMath::RandRange(MinRoomSizePercent, MaxRoomSizePercent);
		Coordinates = ResizeRoom(Coordinates, ResizePercent);
		PartitionedBSPNodes[i]->SetCornerCoordinates(Coordinates);
		DrawBSPNode(World, Coordinates, FColor::Blue);
	}
	//Connect all nodes with hallways
	ConnectNodes(World, FindRootNode(PartitionedBSPNodes[0]));
}

void BSP::DrawBSPNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates, FColor DebugColor)
{
	const FVector UpperLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector UpperRight(Coordinates.LowerRightX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector LowerLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	const FVector LowerRight(Coordinates.LowerRightX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	
	DrawDebugLine(World, UpperLeft, UpperRight, DebugColor, true, -1, 0, 40.f);
	DrawDebugLine(World, UpperRight, LowerRight, DebugColor, true, -1, 0, 40.f);
	DrawDebugLine(World, LowerRight, LowerLeft, DebugColor, true, -1, 0, 40.f);
	DrawDebugLine(World, LowerLeft, UpperLeft, DebugColor, true, -1, 0, 40.f);	
}

/** Traverse Tree Recursively using Depth-first-search preorder to connect all nodes in the tree **/
void BSP::ConnectNodes(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> RootNode)
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

/** Finds the best nodes to connect from both the NodeA side and NodeB side and calls CreateHallway on success **/
bool BSP::ConnectAttempt(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB, ESplitOrientation ConnectOrientation)
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
		bHasOverlap = CalculateHasOverlap(NodeA->GetCornerCoordinates().UpperLeftX,
										 NodeA->GetCornerCoordinates().LowerRightX,
										 NodeB->GetCornerCoordinates().UpperLeftX,
										 NodeB->GetCornerCoordinates().LowerRightX, OverlapStart, OverlapEnd);
	}
	else// if (ConnectOrientation == ESplitOrientation::ESO_Vertical)
		{
		bHasOverlap = CalculateHasOverlap(NodeA->GetCornerCoordinates().UpperLeftY,
										 NodeA->GetCornerCoordinates().LowerRightY,
										 NodeB->GetCornerCoordinates().UpperLeftY,
										 NodeB->GetCornerCoordinates().LowerRightY, OverlapStart, OverlapEnd);
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

void BSP::CreateHallway(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB, int32 OverlapStart, int32 OverlapEnd)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Hallway"));

	TSharedPtr<BSPNode> HallwayNode(new BSPNode());
	FCornerCoordinates HallwayCornerCoordinates;
	
	int32 HallwayRadius = HallwayMinWidth / 2;
	int32 HallwayRandomPos = FMath::RandRange(OverlapStart + HallwayRadius,OverlapEnd - HallwayRadius);
	FColor HallwayColor = FColor::Black;

	if (NodeB->GetCornerCoordinates().UpperLeftY > NodeA->GetCornerCoordinates().LowerRightY)
	{
		HallwayCornerCoordinates.UpperLeftX = HallwayRandomPos - HallwayRadius;
		HallwayCornerCoordinates.UpperLeftY = NodeA->GetCornerCoordinates().LowerRightY;
		HallwayCornerCoordinates.LowerRightX = HallwayRandomPos + HallwayRadius;
		HallwayCornerCoordinates.LowerRightY = NodeB->GetCornerCoordinates().UpperLeftY;
		HallwayColor = FColor::Green;		
	}
	else// if(NodeB->GetCornerCoordinates().UpperLeftX > NodeA->GetCornerCoordinates().LowerRightX)
	{
		HallwayCornerCoordinates.UpperLeftX = NodeB->GetCornerCoordinates().UpperLeftX;
		HallwayCornerCoordinates.UpperLeftY = HallwayRandomPos - HallwayRadius;
		HallwayCornerCoordinates.LowerRightX = NodeA->GetCornerCoordinates().LowerRightX;
		HallwayCornerCoordinates.LowerRightY = HallwayRandomPos + HallwayRadius;
		HallwayColor = FColor::Red;
	}	
	
	HallwayNode->SetCornerCoordinates(HallwayCornerCoordinates);
	//Should Parent node also be the hallways parent? 
	NodeA->GetParentNode()->SetHallwayNode(HallwayNode);
	DrawBSPNode(World, HallwayCornerCoordinates, HallwayColor);
}

TSharedPtr<BSPNode> BSP::FindRootNode(TSharedPtr<BSPNode> InNode)
{
	if (InNode->GetParentNode() != nullptr)
	{
		return FindRootNode(InNode->GetParentNode());
	}
	return InNode;
}

/** Not actual overlap is calculated, but rather what part of 2 sides of different nodes align **/
bool BSP::CalculateHasOverlap(int32 LineStartA, int32 LineEndA, int32 LineStartB, int32 LineEndB,
                                int32& OverlapStart, int32& OverlapEnd)
{
	OverlapStart = TMathUtil<int32>::Max(LineStartA, LineStartB);
	OverlapEnd = TMathUtil<int32>::Min(LineEndA, LineEndB);
	return OverlapEnd - OverlapStart > 0;
}

/** Euclidean distance between the centers of 2 nodes **/
int32 BSP::DistanceBetweenNodes(TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB)
{
	if (NodeA == nullptr || NodeB == nullptr)
	{
		return 0;
	}
	float NodeACenterX = (NodeA->GetCornerCoordinates().LowerRightX + NodeA->GetCornerCoordinates().UpperLeftX) / 2;
	float NodeACenterY = (NodeA->GetCornerCoordinates().LowerRightY + NodeA->GetCornerCoordinates().UpperLeftY) / 2;
	FVector2f NodeACenter = FVector2f(NodeACenterX, NodeACenterY);

	float NodeBCenterX = (NodeB->GetCornerCoordinates().LowerRightX + NodeB->GetCornerCoordinates().UpperLeftX) / 2;
	float NodeBCenterY = (NodeB->GetCornerCoordinates().LowerRightY + NodeB->GetCornerCoordinates().UpperLeftY) / 2;
	FVector2f NodeBCenter = FVector2f(NodeBCenterX, NodeBCenterY);
	
	return FVector2f::Distance(NodeACenter, NodeBCenter);
}




