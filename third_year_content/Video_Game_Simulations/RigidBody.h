#pragma once
#include "../../Common/Vector2.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class RigidBody	{
			friend class GameSimsPhysics;
		public:
			RigidBody();
			~RigidBody();

			// getters and setters of position, velocity and force
			Vector2 GetPosition() const {
				return position;
			}
			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			Vector2 GetVelocity() const {
				return velocity;
			}
			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

				// Getter and Setter for the inverse mass and also for the mass itself for ease
			float GetInverseMass() const {
				return this->inverseMass;
			}
			void SetInverseMass(const float objectInverseMass) {
				this->inverseMass = objectInverseMass;
			}
			void SetMass(const float objectMass) {
				this->inverseMass = float(1 / objectMass);
			}

				// Getter and Setter for the elasticity propriety
			float GetElasticity() const {
				return this->elasticity;
			}
			void SetElasticity(const float objectElasticity) {
				this->elasticity = objectElasticity;
			}

		protected:
			Vector2 position;
			Vector2 velocity;
			Vector2 force;

			float inverseMass;
			float elasticity;

		};
	}
}
