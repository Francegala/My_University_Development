#pragma once
#include <vector>
#include "GameMap.h"

namespace NCL::CSC3222 {
	class SimObject;
	class GameSimsRenderer;
	class GameSimsPhysics;
	class TextureBase;
	class TextureManager;
	class PlayerCharacter;

	class FruitWizardGame {
	public:
		FruitWizardGame();
		~FruitWizardGame();

		void Update(float dt);

			// method used from SimObject to get the tile type (letter) passing through the reference of the FruitWizardGame
		char GetCurrentMapData(int data) {
			return this->currentMap->GetMapData(data);
		}
		GameMap* GetCurrentMap() {
			return this->currentMap;
		}
		void AddNewObject(SimObject* object);

			// Getter for the renderer since we might need to draw colliders and for the Physics engine
		GameSimsRenderer* GetRenderer() const {
			return renderer;
		}

		GameSimsPhysics* Getphysics() const {
			return this->physics;
		}
		
		//	Getter to pass the boolean used from froggo in case the user want to see path finding steps
		bool getShowCollider() {
			return this->showColliders;
		}
			/*
				Methods to update game related stats:
					SumMagicCount	=	adds the parameter to the number of spells
					sumFruitCount	=	since when the user collects 16 fruits, some other 16 fruits get generated,
										it keeps the count of the fruits in game plus how many time it already collected 16 fruits
					SumLifeCount	=	adds life points
					SumPointsCount	=	add points
					SumDustCount	=	keeps track of pixie dust collected so when 4 it can activate pixie mode
					GetMagicCount	=	Getter for spell
					GetDustsCount	=	Returns the list of pixie dust
			*/
		void SumMagicCount(int value) {
			this->magicCount+=value;
		}
		void sumFruitCount(int value) {
			this->fruitCollected = (fruitCompleted * 16) + value;
		}
		void SumLifeCount(int value) {
			this->lives += value;
		}
		void SumPointsCount(int value) {
			this->currentScore += value;
		}
		void SumDustCount(int value) {
			this->dustCount += value;
		}
		int GetMagicCount() const {
			return this->magicCount;
		}
		std::vector<SimObject*> GetDustsCount() const {
			return this->dusts;
		}

		/*
			Flocking:
				I decided to put the flocking method here to be called so when the user click CTRL, it spawns the pixies
				and when calling the update method it adjust the direciton of the pixies.
			This method can be used with any list of SimObjects so it's better to check if lists are empty beforehand as I am doing in the update function too 
		*/
		void Flocking(std::vector<SimObject*> allBoids, std::vector<SimObject*> allBoidsEnemies, float dt);

			// this method manage interactions between two objects/characters
		void CollisionGame(SimObject* sObj1, SimObject* sObj2);

	protected:
		void InitialiseGame();

		TextureManager*		texManager;
		GameSimsRenderer*	renderer;
		GameSimsPhysics*	physics;
		GameMap* currentMap;

		PlayerCharacter* player;

		/*
			showColliders		=	to know if the user wants to see colliders
			notFroggoYet		=	to know if a froggo has already spawned when the user collected 16 fruits
			boolAvoidRepetition	=	boolean used to avoid repetiton when spawning new fruits
			gameover			=	boolean to make appear/disappear gameover text

			fruits, dusts, pixies, guards	=	vectors to keep track of object of the same type in order to create/destroy/change position and interact among them
			fruitCollected, fruitCompleted, dustTimer, gameoverTimer	=	counters to keep track of game stats

		*/
		bool showColliders;
		bool notFroggoYet = false;
		bool boolAvoidRepetition = false;
		bool gameover = false;
		std::vector<SimObject*> fruits;
		std::vector<SimObject*> dusts;
		std::vector<SimObject*> pixies;
		std::vector<SimObject*> guards;
		int fruitCollected = 0;
		int fruitCompleted = 0;
		float dustTimer = 0;
		float gameoverTimer = 0;
		float gameTime;

		int currentScore;
		int magicCount;
		int dustCount;
		int lives;

		std::vector<SimObject*> gameObjects;
		std::vector<SimObject*> newObjects;
	};
}