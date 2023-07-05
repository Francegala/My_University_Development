#pragma once
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "SimObject.h"

	// Circle Shape for Collision Volumes
namespace NCL::CSC3222 {
	class CircleVolume : public CollisionVolume
	{
	public:
		CircleVolume(float radius);
		float GetRadius() const;

	protected:
		float radius;
	};
}




