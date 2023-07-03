#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class CollisionVolume {
		public:
				//Enum Class Type to recognise the type of collides and isolate right collision methods
			enum class Type {
				AABB, Circle
			};
			
			CollisionVolume(Type t);
			~CollisionVolume();
			
				// Getter for Type
			Type getType() {
				return type;
			};

				// Function to draw colliders shapes around object for debugging purposes
			void DrawCollider();

			/*
				Collision detection:
					CheckCollisionC = collision circle cirlce - called by a circle passing as paramenter another circle
					CheckCollisionS = collision AABB AABB - called by a AABB passing as paramenter another AABB
					CheckCollisionCS = collision circle AABB - called by a circle passing as paramenter an AABB
					CheckCollisionSC = collision AABB cirlce - called by an AABB passing as paramenter a circle
				they returns true if there is a collision otherwise false and moreover if two object collides information about the collision get saved in both colliders
			*/
			bool CheckCollisionC(CollisionVolume* s2);
			bool CheckCollisionS(CollisionVolume* s2);
			bool CheckCollisionCS(CollisionVolume* s2);
			bool CheckCollisionSC(CollisionVolume* s2);
			float clamp(float value, float minValue, float maxValue);

				// Getters and Setters for Penetration Distance and Normal in case of a collision (saved in both colliders)
			void SetPenetrationDistance(float pd) {
				this->penetrationDistance = pd;
			}
			float GetPenetrationDistance() {
				return this->penetrationDistance;
			}
			void SetNormal(Vector2 n) {
				this->normal = n;
			}
			Vector2 getNormal() {
				return this->normal;
			}

				// Getter and Setter for a reference to the sim object "owning" the collider when setting the volume
			void SetSim(SimObject* sObj);
			SimObject * GetSim();

		private:
			SimObject* localSO;
			Type type;
			float penetrationDistance;
			Vector2 normal;
		};

	}
}

