#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	class Guard : public SimObject	{
	public:
		//	enum class to have states for the Finite State Machine for the Guard
		enum class State {
			Patroling, Attack // it can either be in patroling mode and walk back and forth
		};

		Guard(PlayerCharacter* p);
		~Guard();

		bool UpdateObject(float dt) override;
			
		// Every time in collision with a spell, the Guard set a flag and starts a timer
		void GetStunned() {
			this->stunned = true;
			this->stunnedTimer = 0;
		}
		bool IsStuned() {
			return this->stunned;
		}

		/*
			State machine:
				Patroling method in patroling state
				Attack method if player is within 3 tiles

			Patroling method uses as parameters dt, needed to calculate positions with the speed ,
					the tile underneath to know if it reached the edge and the distance with the player to know if it needs to change state
			Attach method only uses the distance with the player to know whether to change state
		*/
		void Patroling(float dt, float speed, int tileUnder, int withinPlayer);
		void Attack(int withinPlayer);

	protected:
		//	timer and boolean to know when and how long to stay stunned
		float stunnedTimer;
		bool stunned = false;
		//	save starting position and its relative tile as it can change if falling or if relocated when player dies
		Vector2 startingPoint;
		int tileStartingPosition;
		bool changedPos=false;
		//	save state to know what it's doing and a reference to the player to know if it is within 3 tiles
		State state;
		PlayerCharacter* playerReference;

	};
}