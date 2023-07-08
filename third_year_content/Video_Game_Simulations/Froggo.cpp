#include "Froggo.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"
#include "GameSimsPhysics.h"

using namespace NCL;
using namespace CSC3222;

Vector4 froggoIdleFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoWalkFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoLadderFrames[] = {
	Vector4(0  ,(96 * 4) + 32, 80, 64),
	Vector4(80 ,(96 * 4) + 32, 80, 64),
	Vector4(160,(96 * 4) + 32, 80, 64),
	Vector4(240,(96 * 4) + 32, 80, 64),
	Vector4(320,(96 * 4) + 32, 80, 64),
	Vector4(400,(96 * 4) + 32, 80, 64),
};

Vector4 froggoStunFrames[] = {
	Vector4(0  ,(96 * 8) + 32, 80, 64),
	Vector4(80, (96 * 8) + 32, 80, 64),
	Vector4(160,(96 * 8) + 32, 80, 64),
	Vector4(240,(96 * 8) + 32, 80, 64),
	Vector4(320,(96 * 8) + 32, 80, 64),
	Vector4(400,(96 * 8) + 32, 80, 64),
};

Vector4 froggoAttackFrames[] = {
	Vector4(0  ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(240,(96 * 1) + 32, 80, 64),
};

Vector4 froggoJumpFrames[] = {
	Vector4(0  ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(240,(96 * 7) + 32, 80, 64),
};

	// When creating a new Froggo save when type of game character it is to facilitate interaction among objects and attaching a collider
Froggo::Froggo(PlayerCharacter* p) : SimObject() {
	texture			 = texManager->GetTexture("FruitWizard\\frogman_crown.png");
	animFrameCount	 = 6;
	this->SetCollider(new CircleVolume(13.0f));
	this->SetCharacter(SimObject::Character::Froggo);
	this->playerReference = p;
	this->state = State::Chase; // start from state chase saving the reference of the player
}

Froggo::~Froggo() {
}

bool Froggo::UpdateObject(float dt) {

	// if the user press C from the fruit wizard game class it sets a boolean to show colliders and it shows also steps of path finding
	if(this->game->getShowCollider())for (auto it = path.begin(); it != path.end(); ++it) this->game->GetRenderer()->DrawBox(*it, Vector2(8, 8), Vector4(1, 0, 1, 1));

	
		// assigning a fixed force for making the object move
	float testSpeed = 40 + rand() % 50;
	
	/*
		Checking if the map tile under the Froggo is "empty" (0) to make it fall according to gravity
		and since probably it's jumping use the right animation
	*/
	int tileUnder = ((20 - 1 - (int)(21 - ((this->position.y - (testSpeed * dt)) / 16))) * 30) + this->position.x / 16;
	if (
		((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == '0'
		) {
		animFrameData = froggoJumpFrames[currentanimFrame];
		this->AddForce(Vector2(0, -78));
		timer += dt;// accelerate the process to refresh path finding as usually when it falls it loses track of the steps in the list
	}

	/*
		If the froggo is stunned use a timer to know when it ends and it does not interact with player from the FruitWizardGame class
		being stunned is temporary and in order not to disrupt actions the character was doing, it's outside states, it's like being temporary paused
		froggo can be stunned in any state and then go back at what it was doing
	*/
	if (this->stunned) {
		animFrameData = froggoStunFrames[currentanimFrame];

		if (this->stunnedTimer < 5) {
			stunnedTimer += dt;
		}
		else if (this->stunnedTimer > 5) {
			this->stunned = false;
			this->stunnedTimer = 0;
			iterator = -1;
		}
		return true; // return to pause any action 
	}

	/*
		Refresh the path finding to avoid run across not up to date user position in the map if the user keeps moving 
	*/
	if (this->stunned == false) {
		timer += dt;
		if (timer > 7) {
			iterator = -1;
			timer = 0;
		}
	
	}

	// Compare position with player to know if the player is nearby and it needs to attack
	int withinPlayer = ((((20 - 1 - (int)(21 - ((this->position.y) / 16))) * 30) + this->position.x / 16) -
		(((20 - 1 - (int)(21 - ((((SimObject*)playerReference)->GetPosition().y) / 16))) * 30) + ((SimObject*)playerReference)->GetPosition().x / 16));

	switch (this->state)
	{
	case State::Chase:
		this->Chase(tileUnder,testSpeed, withinPlayer);
		break;
	case State::Attack:
		this->Attack(testSpeed, withinPlayer);
		break;
	case State::Protect:
		this->Protect(tileUnder, testSpeed, withinPlayer);
		break;
	default:
		break;
	}

	return true;
}
/*
	Chase:
	if it needs to refresh the path to the player (has reached last position movign through the list) it calculates a new path
	otherwise it adds a force to the direction of the next position in the list from path finding

	pass to attack mode if player is nearby
	if there is a pixie dust change to protect mode

*/
void NCL::CSC3222::Froggo::Chase(int tileUnder, float testSpeed, int withinPlayer)
{
	animFrameData = froggoWalkFrames[currentanimFrame];
	/* if player within 2 tiles attack mode*/
	if (withinPlayer < 3 && withinPlayer > -3) {
		animFrameData = froggoIdleFrames[currentanimFrame];
		iterator = -1;
		timer = 0;
		this->state = State::Attack;
		return;
	}
	//if there is at least one dust, move to protect state
	if (this->game->GetDustsCount().size() > 0) {
		animFrameData = froggoIdleFrames[currentanimFrame];
		iterator = -1;
		timer = 0;
		this->state = State::Protect;
		return;
	}
	else {
		if (iterator == -1) { // -1 is the starting point it means that pathfinding needs to run
			if (game->GetCurrentMap()->SearchLoop(
				game->GetCurrentMap()->WorldPosToTilePos(this->GetPosition()),
				game->GetCurrentMap()->WorldPosToTilePos(((SimObject*)playerReference)->GetPosition()),
				path)) {
				iterator = 0; // it is used to iterate the list of positions where froggo must go so 0 is the first and then increase
			}
		}
		else {
			Vector2 distance = path[iterator] - this->position;

			// if froggo is close (or has reached) to the location from the path (length because it's a vector) it passes to the next position
			if (distance.Length() < 15) {
				// I noticed that iterating throught the list two by two made the froggo movements more smooth
				// as Froggo does not stop square by square looking like mechanic/robotic moving - stopping - moving
				// but it flows better having a larger direction span
				iterator+=2; // it increases to visit path [0] the first, then path[2] , [4] and so on

				if (iterator >= path.size()) { // if the iterator has passed the size of the list, froggo has reached last position = refresh path
					iterator = -1;
					timer = 0;
					path.clear();
				}
			}
			else {
				/*
					If the Froggo is walking on ladder it changes animation and walks at half speed
				*/
				if (
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'T' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 't' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'L' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'l'
					) {
					animFrameData = froggoLadderFrames[currentanimFrame];
					AddForce((distance.Normalised() * testSpeed)/2);

				}
				else {//get the right animation and direction
					if (this->GetVelocity().x > 3) {
						this->flipAnimFrame = true;
					}
					if (this->GetVelocity().x < 3) {
						this->flipAnimFrame = false;
					}
					AddForce(distance.Normalised() * testSpeed);
				}
			}
		}
	}
}

/*
	Attack:
	if player is movign away pass to chase
	if player is to the right attack to the right otherwise to the left
*/
void NCL::CSC3222::Froggo::Attack(float testSpeed,int withinPlayer)
{
	animFrameData = froggoAttackFrames[currentanimFrame];
	if (withinPlayer > 4 || withinPlayer < -4) {
		animFrameData = froggoIdleFrames[currentanimFrame];
		this->state = State::Chase;
		iterator = -1;
		timer = 0;
		return;
	}
	else {
		if (withinPlayer < 0) {
			this->flipAnimFrame = true;
			this->AddForce(Vector2(testSpeed, 0));
		}
		else {
			this->flipAnimFrame = false;
			this->AddForce(Vector2(-testSpeed, 0));
		}
	}
}

/*
	Protect:
	if player nearby attack or if there is no dust anymore go to chase

	create path not to the player as chase but to the dust
	if is enough close, do not read it as arrived to destination and calculate a new path but stay to the spot to protect the dust
	if player approach, attack has the priority and froggo moves, if player gets the dust, froggo do not stay immobile but goes to chase gain
*/
void NCL::CSC3222::Froggo::Protect(int tileUnder, float testSpeed, int withinPlayer)
{
	animFrameData = froggoWalkFrames[currentanimFrame];
	/* attack is a priority */
	if (withinPlayer < 3 && withinPlayer > -3) {
		animFrameData = froggoIdleFrames[currentanimFrame];
		iterator = -1;
		this->state = State::Attack;
		timer = 0;
		return;
	}
	if (this->game->GetDustsCount().size() == 0) {
		animFrameData = froggoIdleFrames[currentanimFrame];
		iterator = -1;
		this->state = State::Chase;
		timer = 0;
		return;
	}
	else {
		if (iterator == -1) {
			// since there might be more dusts, get randomly a dust piking from zero until last -1 (using % modulo so if there is only one it gets array[zero])
			int element = rand() % this->game->GetDustsCount().size();
			if (game->GetCurrentMap()->SearchLoop(
				game->GetCurrentMap()->WorldPosToTilePos(this->GetPosition()),
				game->GetCurrentMap()->WorldPosToTilePos(
					this->game->GetDustsCount()[element]->GetPosition()
				),
				path)) {
				iterator = 0;
			}
		}
		else {
			// if froggo has reached the dust do not recalculate the path for the same position already reached 
			if (
				path.size() < 3 && // 3 is enough close to spot if player is around and not to activate path finding over and over again
				(abs(path[iterator].y - this->position.y) < 10) &&
				(abs(path[iterator].x - this->position.x) < 10)
				) {
				animFrameData = froggoIdleFrames[currentanimFrame];
				return;
			}

			Vector2 distance = path[iterator] - this->position;
			
			if (distance.Length() < 15) {
				// I noticed that iterating throught the list two by two made the froggo movements more smooth
				// as Froggo does not stop square by square looking like mechanic/robotic moving - stopping - moving
				// but it flows better having a larger direction span
				iterator+=2;

				if (iterator >= path.size()) {
					iterator = -1;
					timer = 0;
					path.clear();
				}
			}
			else {
				/*
					If the Froggo is walking on ladder it changes animation and walks at half speed
				*/
				if (
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'T' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 't' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'L' ||
					((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'l'
					) {
					animFrameData = froggoLadderFrames[currentanimFrame];
					AddForce((distance.Normalised() * testSpeed) / 2);

				}
				else {
					// depending on where is going  use the right animation and direction
					if (this->GetVelocity().x > 3) {
						this->flipAnimFrame = true;
					}
					if (this->GetVelocity().x < -3) {
						this->flipAnimFrame = false;
					}
					AddForce(distance.Normalised() * testSpeed);
				}
			}
		}
	}
}