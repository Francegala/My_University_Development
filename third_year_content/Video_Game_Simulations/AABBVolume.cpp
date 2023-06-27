#include "AABBVolume.h"
#include "SimObject.h"
#include "CollisionVolume.h"
using namespace NCL;
using namespace CSC3222;

	// Constructor calls Collision Volume parent constructor storing the Type and saves the half Width and Height
AABBVolume::AABBVolume(float halfWidthVar, float halfHeightVar) : CollisionVolume(CollisionVolume::Type::AABB)
{
	this->halfWidth = halfWidthVar;
	this->halfHeight = halfHeightVar;
}

	//Getters for the dimentions
float AABBVolume::GetHalfWidth() const
{
	return this->halfWidth;
}

float AABBVolume::GetHalfHeight() const
{
	return this->halfHeight;
}