﻿#include "FloorNode.h"

int32 FloorNode::FloorNodeCount = 0;

FloorNode::FloorNode()
{
	++FloorNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("FloorNode created."));
}

FloorNode::FloorNode(const FCornerCoordinates& Coordinates)
{
	CornerCoordinates.UpperLeftX = Coordinates.UpperLeftX;
	CornerCoordinates.UpperLeftY = Coordinates.UpperLeftY;
	CornerCoordinates.LowerRightX = Coordinates.LowerRightX;
	CornerCoordinates.LowerRightY = Coordinates.LowerRightY;

	++FloorNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("FloorNode created."));
}

FloorNode::~FloorNode()
{
	--FloorNodeCount;
	UE_LOG(LogTemp, Warning, TEXT("FloorNode destroyed."));
	
}