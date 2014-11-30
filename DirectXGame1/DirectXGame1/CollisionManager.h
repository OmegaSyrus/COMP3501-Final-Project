
#pragma once

#include <cmath>

// Class with helper functions to detect collision
class CollisionManager
{
	public:
		static float SHIP_MOVEMENT_CIRCLE_RADIUS;
		static float CIRCLE_CENTER_X;
		static float CIRCLE_CENTER_Y;
		static float CORRIDOR_LENGTH;

	public:
		static bool CheckInCircle(float, float);
		static bool CheckInHorizontalCorridor(float);
		static bool CheckInVerticalCorridor(float);
};