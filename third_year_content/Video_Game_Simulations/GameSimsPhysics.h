#pragma once
#include <vector>
#include "../../Common/Vector2.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		class GameSimsPhysics	{
		public:
			GameSimsPhysics();
			~GameSimsPhysics();

				// Update method changed to use a constant fixed delta time 
			void FixedPhysics(float realDT);
			void Update(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);

				// Getter for the list of all colliders used to investigate potential collisions
			std::vector<CollisionVolume*> GetColliders() {
				return allColliders;
			};
		protected:
			void Integration(float dt);
				// Physics for movements: integration of velocity after integrating accelleration from Force
			void IntegrateVelocity(float dt);

				/*
					For Collision Detection, the method is split in two, a broad phase which uses the list of all colliders to generate a new list
					A narrow phase method using a list containing vectors of colliders passed from the broadphase as parameter
				*/
			void BroadPhase();
			void NarrowPhase(std::vector<std::vector<CollisionVolume*>>	pairsColliders);

				// If in the narrow phase objects collide, this method gets called passing a pair of objects to resolve the collision
			void CollisionResolution(CollisionVolume* cV1, CollisionVolume* cV2);
			float timeRemaining = 0;
			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;
		};
	}
}

