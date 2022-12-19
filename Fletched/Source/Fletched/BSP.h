#pragma once
#include "BSPNode.h"

enum class ESplitOrientation
{
	ESO_Horizontal,
	ESO_Vertical
};

class UWorld;

class BSP
{
public:
	BSP();
	~BSP();

	void Partition();
	int32 CoinFlip();
	float DiceRoll();
	bool ShouldSplitNode(TSharedPtr<BSPNode> InNode, ESplitOrientation Orientation);
	bool SplitAttempt(TSharedPtr<BSPNode> InNode);
	void SplitHorizontal(TSharedPtr<BSPNode> InA, TSharedPtr<BSPNode> InB, TSharedPtr<BSPNode> InC);
	void SplitVertical(TSharedPtr<BSPNode> InA, TSharedPtr<BSPNode> InB, TSharedPtr<BSPNode> InC);
	FCornerCoordinates ResizeRoom(FCornerCoordinates Coordinates, float ResizePercent);
	
	void DrawBSPNodes(TObjectPtr<UWorld> World);
	void DrawBSPNode(TObjectPtr<UWorld> World, FCornerCoordinates Coordinates, FColor DebugColor);

	void ConnectNodes(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> RootNode);	
	bool ConnectAttempt(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB, ESplitOrientation ConnectOrientation);
	void CreateHallway(TObjectPtr<UWorld> World, TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB, int32 OverlapStart, int32 OverlapEnd);
	
	TSharedPtr<BSPNode> FindRootNode(TSharedPtr<BSPNode> InNode);
	bool CalculateHasOverlap(int32 LineStartA, int32 LineEndA, int32 LineStartB, int32 LineEndB,
	                      int32 &OverlapStart, int32 &OverlapEnd);
	int32 DistanceBetweenNodes(TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB);
	
	FORCEINLINE TArray<TSharedPtr<BSPNode>> GetPartitionedBSPNodes() const { return PartitionedBSPNodes; }

private:
	TArray<TSharedPtr<BSPNode>> BSPNodeStack;
	TArray<TSharedPtr<BSPNode>> PartitionedBSPNodes;


	int32 BSPGridSizeX;
	int32 BSPGridSizeY;
	int32 RoomMinX;
	int32 RoomMinY;

	float UnitLength;

	float SplitChance;

	float MinRoomSizePercent;
	float MaxRoomSizePercent;

	int32 HallwayMinWidth;
	int32 MaxConnectAttempts;

	float RandomHallwayChance;
	
};
