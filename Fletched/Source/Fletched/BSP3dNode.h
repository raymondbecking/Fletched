#pragma once

enum class ESplitOrientation3d;

struct FCornerCoordinates3d
{
	int32 BackUpperLeftX;
	int32 BackUpperLeftY;
	int32 BackUpperLeftZ;
	int32 FrontLowerRightX;
	int32 FrontLowerRightY;
	int32 FrontLowerRightZ;
};

class BSP3dNode
{
public:
	BSP3dNode();
	BSP3dNode(const FCornerCoordinates3d& Coordinates);
	~BSP3dNode();

	FORCEINLINE FCornerCoordinates3d GetCornerCoordinates() const { return CornerCoordinates; }
	FORCEINLINE void SetCornerCoordinates(FCornerCoordinates3d Coordinates) { CornerCoordinates = Coordinates; }

	FORCEINLINE static int32 GetNodeCount() { return NodeCount; }

	FORCEINLINE TSharedPtr<BSP3dNode> GetParentNode() const { return ParentNode; }
	FORCEINLINE void SetParentNode(TSharedPtr<BSP3dNode> Parent) { ParentNode = Parent; }

	FORCEINLINE TSharedPtr<BSP3dNode> GetChildNodeA() const { return ChildNodeA; }
	FORCEINLINE TSharedPtr<BSP3dNode> GetChildNodeB() const { return ChildNodeB; }
	FORCEINLINE void SetChildNodes(TSharedPtr<BSP3dNode> NodeA, TSharedPtr<BSP3dNode> NodeB) { ChildNodeA = NodeA, ChildNodeB = NodeB; }

	FORCEINLINE ESplitOrientation3d GetSplitOrientation() const { return SplitOrientation; }
	FORCEINLINE void SetSplitOrientation(ESplitOrientation3d Orientation) { SplitOrientation = Orientation; }

	FORCEINLINE TSharedPtr<BSP3dNode> GetHallwayNode() const { return HallwayNode; }
	FORCEINLINE void SetHallwayNode(TSharedPtr<BSP3dNode> Hallway) { HallwayNode = Hallway; }
private:
	FCornerCoordinates3d CornerCoordinates;

	static int32 NodeCount;

	TSharedPtr<BSP3dNode> ParentNode;
	TSharedPtr<BSP3dNode> ChildNodeA;
	TSharedPtr<BSP3dNode> ChildNodeB;
	TSharedPtr<BSP3dNode> HallwayNode;

	ESplitOrientation3d SplitOrientation;
	
	//TArray<TSharedPtr<BSP3dNode>> ChildNodes;
};
