#pragma once
#include "CollisionVolume.h"
#include "CircleVolume.h"
#include "SimObject.h"

	// Rectangle shape for AABB collision volume.
namespace NCL::CSC3222 {
	class AABBVolume : public CollisionVolume
	{
	public:
		AABBVolume(float halfHeight, float halfWidth);
		float GetHalfWidth() const;
		float GetHalfHeight() const;

	protected:
		float halfWidth, halfHeight;
	};
}
