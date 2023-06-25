/*
	To define the environment using the map,
	tiles representing walls gets created using this object
*/
#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	class Wall : public SimObject {
	public:
		Wall();
		~Wall();

		bool UpdateObject(float dt) override;

	protected:
	};
}