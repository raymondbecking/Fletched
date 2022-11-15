#include "BSPNode.h"

int32 BSPNode::BSPNodeCount = 0;

BSPNode::BSPNode()
{
	++BSPNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSPNode created."));
}

BSPNode::BSPNode(const FCornerCoordinates& Coordinates)
{
	CornerCoordinates.UpperLeftX = Coordinates.UpperLeftX;
	CornerCoordinates.UpperLeftY = Coordinates.UpperLeftY;
	CornerCoordinates.LowerRightX = Coordinates.LowerRightX;
	CornerCoordinates.LowerRightY = Coordinates.LowerRightY;

	++BSPNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSPNode created."));
}

BSPNode::~BSPNode()
{
	--BSPNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSPNode destroyed."));
	
}
