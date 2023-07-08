#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	class Froggo : public SimObject {
	public:
		//	enum class to have states for the Finite State Machine for the Guard
		enum class State {
			Chase, Attack, Protect
		};
		Froggo(PlayerCharacter* p);
		~Froggo();

		bool UpdateObject(float dt) override;

		// If called this method stun the froggo - each time it gets stunned the timer starts again
		void GetStunned() {
			this->stunned = true;
			this->stunnedTimer = 0;
		}
		bool IsStuned() {
			return this->stunned;
		}


		/*
			State machine:
				Chase state when King Froggo use path finding to chase the player
				Attack method if player is within 2 tiles
				Protect anytime there is at least a Pixie Dust in the map

			Both Chase and Protect methods pathfinding to calculate ways to reach either the player or the pixie dust
			Attach method only uses the distance with the player to know whether to change state
		*/
		void Chase(int tileUnder, float testSpeed, int withinPlayer);
		void Attack(float testSpeed, int withinPlayer);
		void Protect(int tileUnder, float testSpeed, int withinPlayer);

	protected:
		float timer;
		float stunnedTimer;
		bool stunned = false;

		// save state to know what it's doing and a reference to the player to know if it is within 3 tiles
		State state;
		// it must be possble to clculate th epath from froggo position to target position + store the path + navigate
		std::vector<Vector2> path;
		int iterator = -1;
		// Froggo needs to know about position of target SimObject
		PlayerCharacter* playerReference;
	};
}