#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Vector4.h"
#include "RigidBody.h"

#include "FruitWizardGame.h"
#include "CollisionVolume.h"
namespace NCL {
	using namespace Maths;

	namespace Rendering {
		class TextureBase;
	}
	namespace CSC3222 {
		class GameSimsRenderer;
		class TextureManager;
		class FruitWizardGame;
		class RigidBody;
		class CollisionVolume;

		class SimObject	: public RigidBody {
		public:
				//	enum class to divide/split all the different types of game characters: it is used to know how they interact one another
			enum class Character {
				Player, Wall, Guard, Fruit, Froggo, PixieDust, Pixie, Spell
			};
			SimObject();
			~SimObject();

				// method to link a collider to a SimObject
			void SetCollider(CollisionVolume* c);

			CollisionVolume* GetCollider() const {
				return collider;
			}

			void	UpdateAnimFrame(float dt);

			virtual bool UpdateObject(float dt) = 0;

			virtual void DrawObject(GameSimsRenderer &r);

			static void InitObjects(FruitWizardGame* game, TextureManager* manager);

				// Method to print to console what kind of Character the SimObject is (used to show collision)
			void PrintEnum();
				// Method to directly access the game reference saved
			FruitWizardGame* GetGame() const {
				return game;
			}
				// Setter and Getter for the Character enum
			void SetCharacter(Character c) {
				this->character = c;
			}
			Character GetCharacter() const {
				return character;
			}
				// If a SimObject is connected to another via a spring this method return the peer object connected using the spring
			SimObject* GetSpringPair() const {
				if (this->springPair != nullptr)return this->springPair;
				else throw "error at pairing spring objects";
			}
				// Method to simulate a spring effect
			void Spring(SimObject* pair);
				
				/* 
					Methods for Flocking
						Alignment: members in a flock should travel in the same direction as each other
						Separation: members should not collide with each other
						Cohesion: members should not move too far away from their flock
						Avoidance: Boids of one type avoid boids of another, here the parameter passes is the 'EnemyBoids' so without checking what type they belong,
									the member calling the funtion will avoid all the members in the list passed as reference
				*/ 
			Vector3 Alignment(std::vector<SimObject*> allBoids);
			Vector3 Separation(std::vector<SimObject*> allBoids);
			Vector3 Cohesion(std::vector<SimObject*> allBoids);
			Vector3 Avoidance(std::vector<SimObject*> allBoidsEnemies);

				// Return the "direction" the SimObject is facing it's a boolean but it can be used to indicate right of left if the image is flipped
			bool GetFlipAnimFrame() {
				return this->flipAnimFrame;
			}
				// Set the flag to instantly delete the SimObject in the update method (if specified)
			void GetErased() {
				this->erased = true;
			};
		protected:

			CollisionVolume*		collider;
			Rendering::TextureBase* texture;

			Vector4			animFrameData;
			int				currentanimFrame;
			bool			flipAnimFrame;
			int				animFrameCount;
			float			frameTime;
			float			frameRate;
			Character		character;		// type of game character
			SimObject*		springPair;		// object paired with to activate spring effect
			bool			erased = false;	// deletion flag to eliminate character

			static TextureManager*  texManager;
			static FruitWizardGame*		game;
		};
	}
}

