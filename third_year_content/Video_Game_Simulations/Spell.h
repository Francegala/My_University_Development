#pragma once
#include "SimObject.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class Spell : public SimObject {
		public:
			Spell(Vector2 direction);
			~Spell();
				//	Every time a spell touches something it needs to change direction and be deleted after 3 bounces
			void Bounce() {
				this->bounces -= 1;
			}

			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;
		private:
			int bounces = 3; // counter per each spell
		};
	}
}

