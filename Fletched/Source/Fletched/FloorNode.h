#pragma once

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
	~FloorNode();

	FORCEINLINE FCornerCoordinates GetCornerCoordinates() const { return CornerCoordinates; }

private:
	FCornerCoordinates CornerCoordinates;
};
