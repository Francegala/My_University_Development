#include "FruitWizardGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "Fruit.h"
#include "Guard.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"
#include "Spell.h"
#include "AABBVolume.h"
#include "CircleVolume.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

FruitWizardGame::FruitWizardGame()	{
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics();
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

FruitWizardGame::~FruitWizardGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void FruitWizardGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;

	renderer->Update(dt);
		// Fixed dt used
	physics->FixedPhysics(dt);

	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		(*i)->UpdateAnimFrame(dt);
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
				// when deleting an object also the collider gets removed
			physics->RemoveCollider((*i)->GetCollider());
			delete (*i);
			 i = gameObjects.erase(i);
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}	
	renderer->DrawString("Score:" + std::to_string(currentScore), 
		Vector2(32, 12), Vector4(1,1,1,1), 100.0f);
	renderer->DrawString("Lives:" + std::to_string(lives), 
		Vector2(144, 12), Vector4(1, 0,0, 1), 100.0f);
	renderer->DrawString("Magic:" + std::to_string(magicCount), 
		Vector2(256, 12), Vector4(1, 0, 0.5f, 1), 100.0f);
	renderer->DrawString("Dust:"  + std::to_string(dustCount), 
		Vector2(366, 12), Vector4(0.5f, 0.3f, 0.8f, 1), 100.0f);
	
		// added new stats to the game
	renderer->DrawString("Fruits Collected:" + std::to_string(fruitCollected),
		Vector2(256, 22), Vector4(1, 1, 1, 1), 70.0f);
	// the user can press C key to see colliders and froggo path
	renderer->DrawString("Press 'C' to see colliders",
		Vector2(32, 22), Vector4(1, 1, 1, 1), 70.0f);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::C)) {
		showColliders = !showColliders;
	}
		// Show colliders if 'C' key is pressed
	if (showColliders) {
		for (auto& c : physics->GetColliders()) {
			c->DrawCollider();
		}
	}

	// Developer cheats options to test the Game
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::D)) {
		magicCount += 50;
		lives += 50;
		dustCount = 4;
	}

	// if player losts all life points a game over message appears
	if (gameover) {
		gameoverTimer += dt;
		renderer->DrawString(" GAME OVER ",
			Vector2(100, 150), Vector4(1, 0, 0, 1), 300.0f);
		if (gameoverTimer >= 3.0f) {
			gameover = false;
			gameoverTimer = 0;
		}
	}

	// game restarts when player dies
	if (lives < 1) {
		gameover = true;
		// Clear all the vectors and assign colliders to the map
		currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, this);
		currentMap->AddColliders(this);

		pixies.clear();
		dusts.clear();
		fruits.clear();
		guards.clear();
		notFroggoYet = false;
		boolAvoidRepetition = false;

		fruitCollected = 0;
		fruitCompleted = 0;
		dustTimer = 0;
		gameTime = 0;

		currentScore = 0;
		magicCount = 4;
		dustCount = 0;
		lives = 3;

		// remove all characters
		for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
			physics->RemoveCollider((*i)->GetCollider());
			delete (*i);
			i = gameObjects.erase(i);
		}
		gameObjects.clear();

		// create new  player
		player = new PlayerCharacter();
		player->SetPosition(Vector2(100, 32));
		AddNewObject(player);
		player->SetMass(1.5f);
		player->SetElasticity(0.1f);


		// randomly spawn fruits and guards again from the beginning, from scratch
		srand(time(NULL));
		for (int i = 1; i <= 4; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* groundFruit = new Fruit();
			groundFruit->SetPosition(Vector2(spawnX, 23));
			fruits.push_back(groundFruit);
			AddNewObject(groundFruit);
		}
		for (int i = 1; i <= 4; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* pixieFruit = new Fruit();
			pixieFruit->SetPosition(Vector2(spawnX, 86));
			fruits.push_back(pixieFruit);
			AddNewObject(pixieFruit);
		}
		for (int i = 1; i <= 4; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* guardFruit = new Fruit();
			guardFruit->SetPosition(Vector2(spawnX, 151));
			fruits.push_back(guardFruit);
			AddNewObject(guardFruit);
		}
		for (int i = 1; i <= 4; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* froggoFruit = new Fruit();
			froggoFruit->SetPosition(Vector2(spawnX, 279));
			fruits.push_back(froggoFruit);
			AddNewObject(froggoFruit);
		}

		for (int i = 0; i <= 3; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Guard* testGuard = new Guard(player);
			switch (i)
			{
			case 0:
				testGuard->SetPosition(Vector2(150, 224));
				break;
			case 1:
				testGuard->SetPosition(Vector2(spawnX, 32));
				break;
			case 2:
				testGuard->SetPosition(Vector2(spawnX, 96));
				break;
			case 3:
				testGuard->SetPosition(Vector2(spawnX, 161));
				break;
			}
			AddNewObject(testGuard);
			testGuard->SetMass(2.5f);
			guards.push_back(testGuard);
		}
	}

	/*
		Each time the user collects 16 new fruits,
		if not already done, new fruits get spawned.
		Since the check gets done every frame and it might be that the user stays with 16 fruits collected for longer time,
		so a boolean is used to avoid spawning fruits at every frame.

		Also a new Guard is added randomly in the map
	*/
	if (fruitCollected % 16 == 0) {
		if (boolAvoidRepetition == true) {
			for (auto f = fruits.begin(); f != fruits.end(); ) {
				(*f)->GetErased();
				f = fruits.erase(f);
			}
			this->SumPointsCount(5000); // add points
			srand(time(NULL));
			for (int i = 1; i <= 4; i++) {
				int spawnX = (180 + 10 * i + (rand() % 195));
				Fruit* groundFruit = new Fruit();
				groundFruit->SetPosition(Vector2(spawnX, 23));
				fruits.push_back(groundFruit);
				AddNewObject(groundFruit);
			}
			for (int i = 1; i <= 4; i++) {
				int spawnX = (180 + 10 * i + (rand() % 195));
				Fruit* pixieFruit = new Fruit();
				pixieFruit->SetPosition(Vector2(spawnX, 86));
				fruits.push_back(pixieFruit);
				AddNewObject(pixieFruit);
			}
			for (int i = 1; i <= 4; i++) {
				int spawnX = (180 + 10 * i + (rand() % 195));
				Fruit* guardFruit = new Fruit();
				guardFruit->SetPosition(Vector2(spawnX, 151));
				fruits.push_back(guardFruit);
				AddNewObject(guardFruit);
			}
			for (int i = 1; i <= 4; i++) {
				int spawnX = (180 + 10 * i + (rand() % 195));
				Fruit* froggoFruit = new Fruit();
				froggoFruit->SetPosition(Vector2(spawnX, 279));
				fruits.push_back(froggoFruit);
				AddNewObject(froggoFruit);
			}
			int spawnX = (180 + (rand() % 195));
			Guard* testGuard = new Guard(player);
			int randomLevel = 1 + rand() % 3;
			switch (randomLevel)
			{
			case 1:
				testGuard->SetPosition(Vector2(spawnX, 32));
				break;
			case 2:
				testGuard->SetPosition(Vector2(spawnX, 96));
				break;
			case 3:
				testGuard->SetPosition(Vector2(spawnX, 161));
				break;
			}
			AddNewObject(testGuard);
			testGuard->SetMass(2.5f);
			guards.push_back(testGuard);
			this->fruitCompleted += 1;
			this->boolAvoidRepetition = false;
		}
	}
		// when collected 16 fruits a Froggo spawns in the top right corner (for the first time and not a multiple as every time the user collects 16 new fruits, new 16 ones appear) 
	if (fruitCollected >= 16 && !notFroggoYet) {
		Froggo* testFroggo = new Froggo(player);
		testFroggo->SetPosition(Vector2(370, 285));
		AddNewObject(testFroggo);
		notFroggoYet = !notFroggoYet;
	}

	/*
		Every 30 seconds there is *a chance* (a chance = 50% probability) of a pixie dust to spawn.
		If the user has not collected 4 dusts, so a new one can spawn,
		the timer keeps running and when reached 30 a coing flips and 50% of possibilities that it spawns randomly in the map
	*/
	if (dustCount < 4) dustTimer += dt;
	if (dustTimer > 30) {
		srand(time(NULL));
		int coin = rand() % 2;
		if (coin == 1) {
			int spawnX = (180 + (rand() % 195));
			int randomLevel =  rand() % 4;
			PixieDust* testDust = new PixieDust();
				switch (randomLevel)
				{
				case 0:
					testDust->SetPosition(Vector2(150, 229));
					break;
				case 1:
					testDust->SetPosition(Vector2(spawnX, 28));
					break;
				case 2:
					testDust->SetPosition(Vector2(spawnX, 91));
					break;
				case 3:
					testDust->SetPosition(Vector2(spawnX, 156));
					break;
				}
				dusts.push_back(testDust);
				AddNewObject(testDust);
		}
		dustTimer = 0;
	}

		// the user when reached 4 dusts can activate pixie mode which spawns 8 pixies randomly in the map
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::CONTROL) && dustCount > 3) {
		dustCount = 0;
		for (int i = 0; i < 8; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			int randomLevel = rand() % 4;
			Pixie* pixie = new Pixie(player);			
				switch (randomLevel)
				{
				case 0:
					pixie->SetPosition(Vector2(150, 229));
					break;
				case 1:
					pixie->SetPosition(Vector2(spawnX, 28));
					break;
				case 2:
					pixie->SetPosition(Vector2(spawnX, 91));
					break;
				case 3:
					pixie->SetPosition(Vector2(spawnX, 156));
					break;
				}
			AddNewObject(pixie);
			pixies.push_back(pixie);
		}
	}
	
	// if pixie mode is active, they should act as a flock
	if (pixies.size() != 0) Flocking(pixies, guards, dt);

	renderer->Render();
}

/*
	Flocking method has as parameters the list of allBoids to move as a group, the list of enemies to avoid and delta time for scaling the direction
			It calls all the three methods to have a better realistic effect and it always checks that the list of Boids is not empty and also 
			that the list of enemies is not empty before calling the avoidance function
*/
void NCL::CSC3222::FruitWizardGame::Flocking(std::vector<SimObject*> allBoids, std::vector<SimObject*> allBoidsEnemies, float dt)
{
	if (allBoids.size() != 0) { // Reynold's Boids
		for (int i = 0; i < allBoids.size(); i++) {
			Vector3 dir;
			dir += allBoids[i]->Alignment(allBoids);
			dir += allBoids[i]->Separation(allBoids);
			dir += allBoids[i]->Cohesion(allBoids);
			if (allBoidsEnemies.size() != 0) dir += allBoids[i]->Avoidance(allBoidsEnemies);
			allBoids[i]->SetPosition(allBoids[i]->GetPosition() + (dir * dt));
		}
	}
}

/*
	After a collision between two objects, this method gets called passing two characters (sim objects),
	so the class can deal with them and choose what happens next
*/
void NCL::CSC3222::FruitWizardGame::CollisionGame(SimObject* sObj1, SimObject* sObj2)
{
	// if Player hits an enemy (either guard or froggo) 
	// ! -> in case enemy is stunned it can be pushed (by a spell as well) so in case guard fall off platform, new position in saved but no interaction / damage gets recorded <- !
	if (
		((sObj1->GetCharacter() == SimObject::Character::Player && sObj2->GetCharacter() == SimObject::Character::Guard) && ((Guard*)sObj2)->IsStuned() == false) ||
		((sObj1->GetCharacter() == SimObject::Character::Guard && sObj2->GetCharacter() == SimObject::Character::Player) && ((Guard*)sObj1)->IsStuned() == false) ||
		((sObj1->GetCharacter() == SimObject::Character::Player && sObj2->GetCharacter() == SimObject::Character::Froggo) && ((Froggo*)sObj2)->IsStuned() == false) ||
		((sObj1->GetCharacter() == SimObject::Character::Froggo && sObj2->GetCharacter() == SimObject::Character::Player) && ((Froggo*)sObj1)->IsStuned() == false)
		) {

		if(lives >1){
		// if froggo hits player close to the starting point it is going to hit over and over until the game crashes
		// so adding a stun if froggo hits player close to starting point to hit in loop
		if (sObj1->GetCharacter() == SimObject::Character::Froggo && sObj2->GetPosition().y < 90) {
			((Froggo*)sObj1)->GetStunned();
		}
		if (sObj2->GetCharacter() == SimObject::Character::Froggo && sObj1->GetPosition().y < 90) {
			((Froggo*)sObj2)->GetStunned();
		}

		// reset player back to start point
		if (sObj1->GetCharacter() == SimObject::Character::Player) sObj1->SetPosition(Vector2(100, 32));
		else sObj2->SetPosition(Vector2(100, 32));

		//if player dies while pixie mode is active all of the remaining pixies will disappear
		for (auto p = pixies.begin(); p != pixies.end(); ) {
			(*p)->GetErased();
			p = pixies.erase(p);
		}

		// fruits which are still in game gets randomly redistribuited
		int remainingFruits = fruits.size();
		for (auto f = fruits.begin(); f != fruits.end(); ) {
			(*f)->GetErased();
			f = fruits.erase(f);
		}
		srand(time(NULL));
		// this division by for and then remaining in case the number is odd is computed to have an even division among all the levels
		// and avoid gathering too many in the same place
		for (int i = 1; i <= (remainingFruits / 4); i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* groundFruit = new Fruit();
			groundFruit->SetPosition(Vector2(spawnX, 23));
			fruits.push_back(groundFruit);
			AddNewObject(groundFruit);
		}
		for (int i = 1; i <= (remainingFruits / 4); i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* pixieFruit = new Fruit();
			pixieFruit->SetPosition(Vector2(spawnX, 86));
			fruits.push_back(pixieFruit);
			AddNewObject(pixieFruit);
		}
		for (int i = 1; i <= (remainingFruits / 4); i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* guardFruit = new Fruit();
			guardFruit->SetPosition(Vector2(spawnX, 151));
			fruits.push_back(guardFruit);
			AddNewObject(guardFruit);
		}
		for (int i = 1; i <= (remainingFruits - fruits.size()); i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Fruit* froggoFruit = new Fruit();
			froggoFruit->SetPosition(Vector2(spawnX, 279));
			fruits.push_back(froggoFruit);
			AddNewObject(froggoFruit);
		}

		// guards still in game RANDOMLY redistribuited
		int remainingGuards = guards.size();
		for (auto g = guards.begin(); g != guards.end(); ) {
			(*g)->GetErased();
			g = guards.erase(g);
		}
		for (int i = 0; i < remainingGuards; i++) {
			int spawnX = (180 + 10 * i + (rand() % 195));
			Guard* testGuard = new Guard(player);
			int randomLevel = rand() % 4;
			// always new random numbers to avoid overlapping
			switch (randomLevel)
			{
			case 0:
				testGuard->SetPosition(Vector2(150 + rand() % 10, 224));
				break;
			case 1:
				testGuard->SetPosition(Vector2(spawnX + rand() % 10, 32));;
				break;
			case 2:
				testGuard->SetPosition(Vector2(spawnX + rand() % 10, 96));
				break;
			case 3:
				testGuard->SetPosition(Vector2(spawnX + rand() % 10, 161));
				break;
			}
			AddNewObject(testGuard);
			testGuard->SetMass(2.5f);
			guards.push_back(testGuard);
		}
	}
		// subtract a life
		this->SumLifeCount(-1);
	}
	// spell hitting enemies knocked backwards due to the impulse and reaction, not adding another effect and not removing collider when hit
	//  it can be 'pushed' (still without damaging player because they're stunned)
	
	// if a spell hits froggo
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Spell && sObj2->GetCharacter() == SimObject::Character::Froggo) ||
		(sObj1->GetCharacter() == SimObject::Character::Froggo && sObj2->GetCharacter() == SimObject::Character::Spell)
		) {
		// remove spell and stun froggo
		if (sObj1->GetCharacter() == SimObject::Character::Spell) {
			sObj1->GetErased();
			((Froggo*)sObj2)->GetStunned();
		}
		else {
			((Froggo*)sObj1)->GetStunned();
			sObj2->GetErased();
		}
	}
	// if spell hits a guard same as before but giving points
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Spell && sObj2->GetCharacter() == SimObject::Character::Guard) ||
		(sObj1->GetCharacter() == SimObject::Character::Guard && sObj2->GetCharacter() == SimObject::Character::Spell)
		) {
		if (sObj1->GetCharacter() == SimObject::Character::Spell) {
			sObj1->GetErased();
			((Guard*)sObj2)->GetStunned();
		}
		else {
			((Guard*)sObj1)->GetStunned();
			sObj2->GetErased();
		}
		// add points
		this->SumPointsCount(200);
	}
	// need to count the bounces of the spell with wall or flooring since after 3 it gets destroyed
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Spell && sObj2->GetCharacter() == SimObject::Character::Wall) ||
		(sObj1->GetCharacter() == SimObject::Character::Wall && sObj2->GetCharacter() == SimObject::Character::Spell)
		) {
		if (sObj1->GetCharacter() == SimObject::Character::Spell) {
			((Spell*)sObj1)->Bounce();
		}
		else {
			((Spell*)sObj2)->Bounce();
		}
	}
	// Player can collect fruits
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Player && sObj2->GetCharacter() == SimObject::Character::Fruit) ||
		(sObj1->GetCharacter() == SimObject::Character::Fruit && sObj2->GetCharacter() == SimObject::Character::Player)
		) {
	// Destroy the fruit and remove from the list used to count them
		if (sObj1->GetCharacter() == SimObject::Character::Fruit) 
		{
			sObj1->GetErased();
			fruits.erase(std::remove(fruits.begin(), fruits.end(), sObj1), fruits.end());
		}
		else {
			sObj2->GetErased();
			fruits.erase(std::remove(fruits.begin(), fruits.end(), sObj2), fruits.end());
		}
	// give points and show user how many collected 
	// not summing +1 to a counter since sometime it might happen that it adds it multiple times
	// but subtracting the number of fruits remainin in the game from  16 (maximum available)
		this->boolAvoidRepetition = true;
		this->sumFruitCount(16 - fruits.size());
		this->SumPointsCount(1000);
	}
	// Player can collect pixie dust
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Player && sObj2->GetCharacter() == SimObject::Character::PixieDust) ||
		(sObj1->GetCharacter() == SimObject::Character::PixieDust && sObj2->GetCharacter() == SimObject::Character::Player)
		) {
		//remove pixiedust object and give points
		if (sObj1->GetCharacter() == SimObject::Character::PixieDust)
		{
			dusts.erase(std::remove(dusts.begin(), dusts.end(), sObj1), dusts.end());
			sObj1->GetErased();
		}
		else {
			dusts.erase(std::remove(dusts.begin(), dusts.end(), sObj2), dusts.end());
			sObj2->GetErased();
		}
		this->SumDustCount(1);
		this->SumPointsCount(500);
	}
	//if pixie mode is active and pixie spawned, colliding a player with them receives points and spells
	else if (
		(sObj1->GetCharacter() == SimObject::Character::Player && sObj2->GetCharacter() == SimObject::Character::Pixie) ||
		(sObj1->GetCharacter() == SimObject::Character::Pixie && sObj2->GetCharacter() == SimObject::Character::Player)
		) {
		if (sObj1->GetCharacter() == SimObject::Character::Pixie)
		{
			pixies.erase(std::remove(pixies.begin(), pixies.end(), sObj1), pixies.end());
			sObj1->GetErased();
		}
		else {
			pixies.erase(std::remove(pixies.begin(), pixies.end(), sObj2), pixies.end());
			sObj2->GetErased();
		}
		this->SumMagicCount(1);
		this->SumPointsCount(1000);
	}
}

void FruitWizardGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();
		// Clear all the vectors and assign colliders to the map
	pixies.clear();
	dusts.clear();
	fruits.clear();
	guards.clear();
	notFroggoYet = false;
	boolAvoidRepetition = false;

	currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, this);
	currentMap->AddColliders(this);
	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetPosition(Vector2(100, 32));
	AddNewObject(player);
		// Assign a mass and elasticity to the player
	player->SetMass(1.5f);
	player->SetElasticity(0.1f);
	
		// randomly spawn fruits and guards
	srand(time(NULL));
	for (int i = 1; i <= 4; i++) {
		int spawnX = (180 + 10 * i + (rand() % 195));
		Fruit* groundFruit = new Fruit();
		groundFruit->SetPosition(Vector2(spawnX, 23));
		fruits.push_back(groundFruit);
		AddNewObject(groundFruit);
	}
	for (int i = 1; i <= 4; i++) {
		int spawnX = (180 + 10 * i + (rand() % 195));
		Fruit* pixieFruit = new Fruit();
		pixieFruit->SetPosition(Vector2(spawnX, 86));
		fruits.push_back(pixieFruit);
		AddNewObject(pixieFruit);
	}
	for (int i = 1; i <= 4; i++) {
		int spawnX = (180 + 10 * i + (rand() % 195));
		Fruit* guardFruit = new Fruit();
		guardFruit->SetPosition(Vector2(spawnX, 151));
		fruits.push_back(guardFruit);
		AddNewObject(guardFruit);
	}
	for (int i = 1; i <= 4; i++) {
		int spawnX = (180 + 10 * i + (rand() % 195));
		Fruit* froggoFruit = new Fruit();
		froggoFruit->SetPosition(Vector2(spawnX, 279));
		fruits.push_back(froggoFruit);
		AddNewObject(froggoFruit);
	}

	for (int i = 0; i <= 3; i++) {
		int spawnX = (180 + 10 * i + (rand() % 195));
		Guard* testGuard = new Guard(player);
		switch (i)
		{
		case 0:
			testGuard->SetPosition(Vector2(150, 224));
			break;
		case 1:
			testGuard->SetPosition(Vector2(spawnX, 32));
			break;
		case 2:
			testGuard->SetPosition(Vector2(spawnX, 96));
			break;
		case 3:
			testGuard->SetPosition(Vector2(spawnX, 161));
			break;
		}
		AddNewObject(testGuard);
		testGuard->SetMass(2.5f);
		guards.push_back(testGuard);
	}

	fruitCollected	= 0;
	fruitCompleted	= 0;
	dustTimer		= 0;
	gameTime		= 0;

	currentScore	= 0;
	magicCount		= 4;
	dustCount		= 0;
	lives			= 3;
}

void FruitWizardGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}