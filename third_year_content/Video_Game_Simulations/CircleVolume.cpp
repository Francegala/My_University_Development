#include "CircleVolume.h"
#include "SimObject.h"
#include "CollisionVolume.h"
using namespace NCL;
using namespace CSC3222;

/*
	Parent Constructor called first and saves Circle type
	Radius of the circle saved as well
*/
CircleVolume::CircleVolume(float radiusVar) : CollisionVolume(CollisionVolume::Type::Circle)
{
	this->radius = radiusVar;
}

	// Getter for the radius
float CircleVolume::GetRadius() const
{
	return this->radius;
}
