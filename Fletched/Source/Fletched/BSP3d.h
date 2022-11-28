#pragma once
#include "BSP3dNode.h"

enum class ESplitOrientation3d
{
	ESO_Horizontal,
	ESO_Vertical,
	ESO_Depth
};

class UWorld;

class BSP3d
{
public:
	BSP3d();
	~BSP3d();

	void Partition();
	int32 CoinFlip();
	float DiceRoll();
	bool ShouldSplitNode(TSharedPtr<BSP3dNode> InNode, ESplitOrientation3d Orientation);
	bool SplitAttempt(TSharedPtr<BSP3dNode> InNode);
	void SplitHorizontal(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC);
	void SplitVertical(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC);
	void SplitDepth(TSharedPtr<BSP3dNode> InA, TSharedPtr<BSP3dNode> InB, TSharedPtr<BSP3dNode> InC);
	FCornerCoordinates3d ResizeRoom(FCornerCoordinates3d Coordinates, float ResizePercent);
	
	void DrawBSPNodes(TObjectPtr<UWorld> World);
	void DrawBSPNode(TObjectPtr<UWorld> World, FCornerCoordinates3d Coordinates, FColor DebugColor);

	void ConnectNodes(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> RootNode);	
	bool ConnectAttempt(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB, ESplitOrientation3d ConnectOrientation);
	void CreateHallway(TObjectPtr<UWorld> World, TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB, int32 OverlapStartX, int32 OverlapEndX, int32 OverlapStartY, int32 OverlapEndY);
	
	TSharedPtr<BSP3dNode> FindRootNode(TSharedPtr<BSP3dNode> InNode);
	bool CalculateHasOverlap(int32 X1NodeA, int32 X2NodeA, int32 Y1NodeA, int32 Y2NodeA,
							 int32 X1NodeB, int32 X2NodeB, int32 Y1NodeB, int32 Y2NodeB,
	                         int32& OverlapStartX, int32& OverlapEndX, int32& OverlapStartY, int32& OverlapEndY);
	int32 DistanceBetweenNodes(TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB);
	
	FORCEINLINE TArray<TSharedPtr<BSP3dNode>> GetPartitionedNodes() const { return PartitionedNodes; }

private:
	TArray<TSharedPtr<BSP3dNode>> NodeStack;
	TArray<TSharedPtr<BSP3dNode>> PartitionedNodes;


	int32 BSPGridSizeX;
	int32 BSPGridSizeY;
	int32 BSPGridSizeZ;
	int32 RoomMinX;
	int32 RoomMinY;
	int32 RoomMinZ;

	float UnitLength;

	float SplitChance;

	float MinRoomSizePercent;
	float MaxRoomSizePercent;

	int32 HallwayMinWidth;
	int32 MaxConnectAttempts;

	float RandomHallwayChance;
	
};
