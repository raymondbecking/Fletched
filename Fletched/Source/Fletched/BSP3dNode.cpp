#include "BSP3dNode.h"

int32 BSP3dNode::NodeCount = 0;

BSP3dNode::BSP3dNode()
{
	++NodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSP3dNode created."));
}

BSP3dNode::BSP3dNode(const FCornerCoordinates& Coordinates)
{
	CornerCoordinates.BackUpperLeftX = Coordinates.BackUpperLeftX;
	CornerCoordinates.BackUpperLeftY = Coordinates.BackUpperLeftY;
	CornerCoordinates.BackUpperLeftZ = Coordinates.BackUpperLeftZ;
	CornerCoordinates.FrontLowerRightX = Coordinates.FrontLowerRightX;
	CornerCoordinates.FrontLowerRightY = Coordinates.FrontLowerRightY;
	CornerCoordinates.FrontLowerRightZ = Coordinates.FrontLowerRightZ;

	++NodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSP3dNode created."));
}

BSP3dNode::~BSP3dNode()
{
	--NodeCount;
	UE_LOG(LogTemp, Warning, TEXT("BSP3dNode destroyed."));
	
}
