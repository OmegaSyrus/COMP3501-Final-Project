#include "pch.h"
#include "CollisionManager.h"

float CollisionManager::SHIP_MOVEMENT_CIRCLE_RADIUS = 0.25f;
float CollisionManager::CIRCLE_CENTER_X = 0.5;
float CollisionManager::CIRCLE_CENTER_Y = 0.5;
float CollisionManager::CORRIDOR_LENGTH = 0.1f;

bool CollisionManager::CheckInCircle(float x, float y)
{
	float squareDistance = pow(CIRCLE_CENTER_X - x, 2.0f) + pow(CIRCLE_CENTER_Y - y, 2.0f);

	return squareDistance <= pow(SHIP_MOVEMENT_CIRCLE_RADIUS, 2.0f);
}

bool CollisionManager::CheckInHorizontalCorridor(float mouseCoordinate)
{
	if ((mouseCoordinate < (CIRCLE_CENTER_X + CORRIDOR_LENGTH))
		&&
		(mouseCoordinate > (CIRCLE_CENTER_X - CORRIDOR_LENGTH)))
		return true;

	return false;
}

bool CollisionManager::CheckInVerticalCorridor(float mouseCoordinate)
{
	if ((mouseCoordinate < (CIRCLE_CENTER_Y + CORRIDOR_LENGTH))
		&&
		(mouseCoordinate >(CIRCLE_CENTER_Y - CORRIDOR_LENGTH)))
		return true;

	return false;
}