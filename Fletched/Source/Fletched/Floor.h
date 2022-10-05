#pragma once
#include "FloorNode.h"

enum class ESplitOrientation
{
	ESO_Horizontal,
	ESO_Vertical
};

class UWorld;

class Floor
{
public:
	Floor();
	~Floor();

	void Partition();
	int32 CoinFlip();
	float DiceRoll();
	bool ShouldSplitNode(TSharedPtr<FloorNode> InNode, ESplitOrientation Orientation);
	bool SplitAttempt(TSharedPtr<FloorNode> InNode);
	void SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);
	void SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);
	FCornerCoordinates ResizeRoom(FCornerCoordinates Coordinates, float ResizePercent);
	
	void DrawFloorNodes(TObjectPtr<UWorld> World);
	void DrawFloorNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates);

	FORCEINLINE TArray<TSharedPtr<FloorNode>> GetPartitionedFloor() const { return PartitionedFloor; }

private:
	TArray<TSharedPtr<FloorNode>> FloorNodeStack;
	TArray<TSharedPtr<FloorNode>> PartitionedFloor;

	int32 FloorGridSizeX;
	int32 FloorGridSizeY;
	int32 RoomMinX;
	int32 RoomMinY;

	float UnitLength;

	float SplitChance;

	float MinRoomSizePercent;
	
};
