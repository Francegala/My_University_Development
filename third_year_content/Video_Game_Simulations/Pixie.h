#pragma once
#include "SimObject.h"
#include "PlayerCharacter.h"

namespace NCL::CSC3222 {
	class Pixie : public SimObject	{
	public:
			// Pixies are constantly coonected with the player using a spring and to set this connection I am passing the player object as reference to the constructor
		Pixie(PlayerCharacter* p);
		~Pixie();

		bool UpdateObject(float dt) override;

	protected:
			// Player reference to be used when calling the Spring function to have a constant connection with the player even if the pixie collides with something else and has a short spring collision resolution with some othe object
		PlayerCharacter* playerReference;
	};
}