#include "Guard.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"
#include "GameSimsPhysics.h"

using namespace NCL;
using namespace CSC3222;

Vector4 guardIdleFrames[] = {
	Vector4(384,32, 64, 32),
	Vector4(448,32, 64, 32),	
	Vector4(512,32, 64, 32),	
	Vector4(576,32, 64, 32),	
	Vector4(448,64, 64, 32),	
	Vector4(512,64, 64, 32),
};

Vector4 guardWalkFrames[] = {
	Vector4(384,96, 64, 32),
	Vector4(448,96, 64, 32),
	Vector4(512,96, 64, 32),
	Vector4(576,96, 64, 32),
	Vector4(640,96, 64, 32),
	Vector4(704,96, 64, 32),
};

Vector4 guardAttackFrames[] = {
	Vector4(384,128, 64, 32),
	Vector4(448,128, 64, 32),
	Vector4(512,128, 64, 32),
	Vector4(576,128, 64, 32),
	Vector4(640,128, 64, 32),
	Vector4(704,128, 64, 32),
};

Vector4 guardStunFrames[] = {
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
};
/*
	Constructor adds a collider and define type of character
	it also saves the starting position tha can change during the game and the state with player reference for a better interaction
*/
Guard::Guard(PlayerCharacter* p) : SimObject() {
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount	= 6;
	this->SetCollider(new CircleVolume(16.0f));
	this->SetCharacter(SimObject::Character::Guard);
	this->startingPoint = this->GetPosition();
	this->state = State::Patroling;
	this->playerReference = p;
}

Guard::~Guard() {
}

bool Guard::UpdateObject(float dt) {
		// if flag set, object deleted
	if (this->erased)return false;
		// base velocity to add as force
		// a random value gets added as when relocating after a player death more guards might end at the same random place so a variation in velocity results into difference in position due to this variation
	float speed = 40 + rand() % 50;

	// save position in case it changes when reallocating them with a skipped iteration of 1 "LAG" (achieved with the boolean) so the new position is for sure final position "when reached the ground"
	if (changedPos == true) {
		this->startingPoint = this->GetPosition();
		this->tileStartingPosition = ((20 - 1 - (int)(21 - ((this->position.y - (speed * dt)) / 16))) * 30) + this->position.x / 16;
		changedPos = false;
	}

	// if starting point has changed, change bool so with "lag" it can change position
	if (this->startingPoint.y != this->GetPosition().y) {
		changedPos = true;
	}

		// constant check of the tile underneath for gravity force if 'empty space - nothing' add gravity
	int tileUnder = ((20 - 1 - (int)(21 - ((this->position.y - (speed * dt)) / 16))) * 30) + this->position.x / 16;

	if (
		((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == '0'
		) {
		this->AddForce(Vector2(0, -98));
	}

	// return so it's not in a state, stunning it's like outside the state machine so in this way after being stunned it can go back to the the same state/action
	// it's something temporary like being paused that does not change the action started that can applied at any time
	if (this->stunned) {
		animFrameData = guardStunFrames[currentanimFrame];
		if (this->stunnedTimer < 5) {
			stunnedTimer += dt;
		}
		else if (this->stunnedTimer > 5) {
			this->stunned = false;
			this->stunnedTimer = 0;
		}
		return true;
	}

	// both states needs to know the position of the player so calculated beforehand and passed as parameter
	int withinPlayer = ((((20 - 1 - (int)(21 - ((this->position.y) / 16))) * 30) + this->position.x / 16) -
		(((20 - 1 - (int)(21 - ((((SimObject*)playerReference)->GetPosition().y) / 16))) * 30) + ((SimObject*)playerReference)->GetPosition().x / 16));
	

	//	Turn around  when moved more than 10 tiles from starting point
	//	to make it turn, it changes direction changing the flip animation
	//	it is outside a specific state beacuase the guard goes back to patroling state and changes direction if it has moved more than 10 tiles in both cases when patroling or when attacking
	if (!stunned &&
		(tileUnder == tileStartingPosition + 10 && this->flipAnimFrame == true) || (tileUnder == tileStartingPosition - 10 && this->flipAnimFrame == false)
		) {
		this->state = State::Patroling; // if it was in attack mode go back patrolling as specified if already patroling it's ok
		if (this->flipAnimFrame == true) {
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = false;
		}
		else {
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = true;
		}
	}

	switch (this->state)
	{
	case State::Patroling:
		this->Patroling(dt, speed, tileUnder, withinPlayer);
		break;
	case State::Attack:
		this->Attack(withinPlayer);
		break;
	default:
		break;
	}
	
	// if not stunned it uses the rotation to maintain a direction and walk straight
	if (this->stunned == false) {
		if (this->flipAnimFrame == true) {
			this->AddForce(Vector2(speed, 0));
		}
		else {
			this->AddForce(Vector2(-speed, 0));
		}
	}
	return true;
}


//	First state, Patroling = it uses the walk animation and it changes states if the user is within 3 tiles and it is not about to go away
void NCL::CSC3222::Guard::Patroling(float dt, float speed, int tileUnder, int withinPlayer)
{
	// pass in attack mode if player is nearby
	animFrameData = guardWalkFrames[currentanimFrame];
	if (
		(withinPlayer < 4 && withinPlayer > -4) &&
		((tileUnder - tileStartingPosition < 8) || (tileUnder - tileStartingPosition >  -8))
			) {
		this->state = State::Attack;
		return;
	}

	// Turn around when reached an edge checking the tile under the Guard and the one on the right and on the left to know if reached the rightmost edge or the left one
	// to make it turn, it changes direction changing the flip animation
	if (!stunned &&
		((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'H' ||
		((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'F'
		) {
		if (((FruitWizardGame*)this->game)->GetCurrentMapData(((20 - 1 - (int)(21 - ((this->position.y - (speed * dt)) / 16))) * 30) + this->position.x / 16 + 1) == '0') {// if the tile to the right is "edge of platform"
			//turn around
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = false;
		}
		else {// if "left is edge of platform, turn"
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = true;
		}
	}

	//Turn around when reached the wall
	if (!stunned &&
		((((FruitWizardGame*)this->game)->GetCurrentMapData(((20 - 1 - (int)(20 - (this->position.y / 16))) * 30) + 1 + this->position.x / 16) == 'W') && this->flipAnimFrame == true) ||
		((((FruitWizardGame*)this->game)->GetCurrentMapData(((20 - 1 - (int)(20 - (this->position.y / 16))) * 30) - 1 + this->position.x / 16) == 'W') && this->flipAnimFrame == false)
		) {
		if (this->flipAnimFrame == true) {
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = false;
		}
		else {
			animFrameData = guardIdleFrames[currentanimFrame];
			this->flipAnimFrame = true;
		}
	}
}

// Attack state use the attack animation and goes back to patroling if the player is went away or turns to chase the player
void NCL::CSC3222::Guard::Attack(int withinPlayer)
{
	animFrameData = guardAttackFrames[currentanimFrame];
	if (withinPlayer > 4 || withinPlayer < -4){ // if player has moved away go back patroling
		this->state = State::Patroling;
	}
	else {
		if (withinPlayer < 0) {
			this->flipAnimFrame = true;
		}
		else {
			this->flipAnimFrame = false;
		}
	}
}
