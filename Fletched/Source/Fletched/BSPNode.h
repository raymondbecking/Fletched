#pragma once

enum class ESplitOrientation;

struct FCornerCoordinates
{
	int32 UpperLeftX;
	int32 UpperLeftY;
	int32 LowerRightX;
	int32 LowerRightY;
};

class BSPNode
{
public:
	BSPNode();
	BSPNode(const FCornerCoordinates& Coordinates);
	~BSPNode();

	FORCEINLINE FCornerCoordinates GetCornerCoordinates() const { return CornerCoordinates; }
	FORCEINLINE void SetCornerCoordinates(FCornerCoordinates Coordinates) { CornerCoordinates = Coordinates; }

	FORCEINLINE static int32 GetNodeCount() { return BSPNodeCount; }

	FORCEINLINE TSharedPtr<BSPNode> GetParentNode() const { return ParentNode; }
	FORCEINLINE void SetParentNode(TSharedPtr<BSPNode> Parent) { ParentNode = Parent; }

	FORCEINLINE TSharedPtr<BSPNode> GetChildNodeA() const { return ChildNodeA; }
	FORCEINLINE TSharedPtr<BSPNode> GetChildNodeB() const { return ChildNodeB; }
	FORCEINLINE void SetChildNodes(TSharedPtr<BSPNode> NodeA, TSharedPtr<BSPNode> NodeB) { ChildNodeA = NodeA, ChildNodeB = NodeB; }

	FORCEINLINE ESplitOrientation GetSplitOrientation() const { return SplitOrientation; }
	FORCEINLINE void SetSplitOrientation(ESplitOrientation Orientation) { SplitOrientation = Orientation; }

	FORCEINLINE TSharedPtr<BSPNode> GetHallwayNode() const { return HallwayNode; }
	FORCEINLINE void SetHallwayNode(TSharedPtr<BSPNode> Hallway) { HallwayNode = Hallway; }
private:
	FCornerCoordinates CornerCoordinates;

	static int32 BSPNodeCount;

	TSharedPtr<BSPNode> ParentNode;
	TSharedPtr<BSPNode> ChildNodeA;
	TSharedPtr<BSPNode> ChildNodeB;
	TSharedPtr<BSPNode> HallwayNode;

	ESplitOrientation SplitOrientation;
	
	//TArray<TSharedPtr<BSPNode>> ChildNodes;
};
