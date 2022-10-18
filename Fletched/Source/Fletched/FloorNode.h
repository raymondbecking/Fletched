#pragma once

enum class ESplitOrientation;

struct FCornerCoordinates
{
	int32 UpperLeftX;
	int32 UpperLeftY;
	int32 LowerRightX;
	int32 LowerRightY;
};

class FloorNode
{
public:
	FloorNode();
	FloorNode(const FCornerCoordinates& Coordinates);
	~FloorNode();

	FORCEINLINE FCornerCoordinates GetCornerCoordinates() const { return CornerCoordinates; }
	FORCEINLINE void SetCornerCoordinates(FCornerCoordinates Coordinates) { CornerCoordinates = Coordinates; }

	FORCEINLINE static int32 GetNodeCount() { return FloorNodeCount; }

	FORCEINLINE TSharedPtr<FloorNode> GetParentNode() const { return ParentNode; }
	FORCEINLINE void SetParentNode(TSharedPtr<FloorNode> Parent) { ParentNode = Parent; }

	FORCEINLINE TSharedPtr<FloorNode> GetChildNodeA() const { return ChildNodeA; }
	FORCEINLINE TSharedPtr<FloorNode> GetChildNodeB() const { return ChildNodeB; }
	FORCEINLINE void SetChildNodes(TSharedPtr<FloorNode> NodeA, TSharedPtr<FloorNode> NodeB) { ChildNodeA = NodeA, ChildNodeB = NodeB; }

	FORCEINLINE ESplitOrientation GetSplitOrientation() const { return SplitOrientation; }
	FORCEINLINE void SetSplitOrientation(ESplitOrientation Orientation) { SplitOrientation = Orientation; }

	FORCEINLINE TSharedPtr<FloorNode> GetHallwayNode() const { return HallwayNode; }
	FORCEINLINE void SetHallwayNode(TSharedPtr<FloorNode> Hallway) { HallwayNode = Hallway; }
private:
	FCornerCoordinates CornerCoordinates;

	static int32 FloorNodeCount;

	TSharedPtr<FloorNode> ParentNode;
	TSharedPtr<FloorNode> ChildNodeA;
	TSharedPtr<FloorNode> ChildNodeB;
	TSharedPtr<FloorNode> HallwayNode;

	ESplitOrientation SplitOrientation;
	
	//TArray<TSharedPtr<FloorNode>> ChildNodes;
};
