#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"
#include "Spell.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 runFrames[] = {
	Vector4(64,160, 32, 32),
	Vector4(96,160, 32, 32),
	Vector4(128,160, 32, 32),
	Vector4(160,160, 32, 32),
	Vector4(192,160, 32, 32),
	Vector4(224,160, 32, 32),
};

Vector4 attackFrames[] = {
	Vector4(128,288, 32, 30),
	Vector4(64,224, 32, 32),
	Vector4(160,288, 32, 30),
	Vector4(96,224, 32, 32),
	Vector4(192,288, 32, 29),
	Vector4(64,256, 32, 32)
};

Vector4 idleFrames[] = {
	Vector4(64,128, 32, 32),
	Vector4(96,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(160,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(224,128, 32, 32)
};

Vector4 fallFrames[] = {
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32)
};

Vector4 deathFrames[] = {
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),	
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
};

Vector4 ladderFrames[] = {//Not an ideal spritesheet for ladders!
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
};

		// When calling the construction it also adds a collider of Circle Shape and it defines that it is a Player Character so the SimObject knows how to interact with other objects
PlayerCharacter::PlayerCharacter() : SimObject() {
	currentAnimState	= PlayerState::Left;
	texture				= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount		= 6;
	this->SetCollider(new CircleVolume(12.0f));			// when adding a circle collider it must specified the radius
	this->SetCharacter(SimObject::Character::Player);	// what type of game object this is
}

PlayerCharacter::~PlayerCharacter() {

}

bool PlayerCharacter::UpdateObject(float dt) {
	if (this->erased)return false; // if set as deletable, it must be erased

		// base velocity to apply as force
	float testSpeed = 200;
	Vector4* animSource = idleFrames;

		/*	the Character always needs to know on what tile is at the moment at the tile under it
			 starting point has y = ~32 so ~32/16 = ~2 
			 (this->position.y / 16) -> 2
			 (int)(20 - (this->position.y / 16)) -> 18
			  ((20 - 1 - (int)(20 - (this->position.y / 16)))) -> 2

			  but when (this->position.y / 16) -> 2.1
			  (int)(20 - (this->position.y / 16)) -> is 17 not 18 anymore
				so ((20 - 1 - (int)(20 - (this->position.y / 16)))) -> now equal to 3 it sees that it's going up

				IN THIS WAY IT ROUNDS UP BEFORE

				 (int)(20 - (this->position.y / 16)) it calculates from the top how many tiles it is down = 20 -2 = 18
				 20 - 1 since it starts from zero
				 19 - 18 = 1
				 1 * 30 total height + x
		*/ 

	int tileIndex = ((20 - 1 - (int)(20 - (this->position.y / 16))) * 30) + this->position.x / 16;
	int tileUnder = ((20 - 1 - (int)(21 - ((this->position.y - (testSpeed * dt)) / 16))) * 30) + this->position.x / 16;

	// apply gravity if it is on empty space
	if (
		((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == '0'
		) {
		this->AddForce(Vector2(0, -98)); // 98 from real gravity 9.8N 
	}
	if (currentAnimState == PlayerState::Attack) {
		animSource = attackFrames;
		if (currentanimFrame >= 5) {
			currentAnimState = PlayerState::Idle;
		}
	}
	else {
		currentAnimState = PlayerState::Idle;
		/* when pressing keys force is added to teh right direction and player faces the right direction too*/
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			animSource = runFrames;
				currentAnimState = PlayerState::Left;
				this->AddForce(Vector2(-testSpeed, 0));
			flipAnimFrame = true;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			animSource = runFrames;
				currentAnimState = PlayerState::Right;
				this->AddForce(Vector2(testSpeed, 0));
			flipAnimFrame = false;
		}
			// The player can go UP or DOWN only if there is a specific tile indicating the ladder with a lower speed
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			animSource = ladderFrames;
			if (
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 'b' || 
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 'B'	|| 
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 'l' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 'L'	||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 't' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileIndex) == 'T'	||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 't' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'T' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'l' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'L'
				){
				currentAnimState = PlayerState::Idle;
				this->AddForce(Vector2(0, testSpeed/2));
			}
			flipAnimFrame = false;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			animSource = ladderFrames;
			if (
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'T' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 't' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'l' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'L'	||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'b' ||
				((FruitWizardGame*)this->game)->GetCurrentMapData(tileUnder) == 'B'
				) {
				currentAnimState = PlayerState::Idle;
				this->AddForce(Vector2(0, -testSpeed/2));
			}
			flipAnimFrame = true;
		}
			// The player can shoot a spell if has some magic left pressing space
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && ((FruitWizardGame*)this->game)->GetMagicCount()>0) {
			currentAnimState = PlayerState::Attack;
			currentanimFrame = 0;
				// Depending on where the player is facing it shoots a spell right or left changing the force applied
				// It creates a spell object giving specific value on position and direction in the constructor and random mass to play with speed
			Spell* newSpell;
			if (flipAnimFrame == true) {
				newSpell = new Spell(Vector2(-50, 50));
				newSpell->SetPosition(Vector2(this->GetPosition().x - 24, this->GetPosition().y));
				srand(time(NULL));
				newSpell->SetMass(0.4f + (rand() % 2));
			}
			else {
				newSpell = new Spell(Vector2(50, 50));
				newSpell->SetPosition(Vector2(this->GetPosition().x + 24, this->GetPosition().y));
				srand(time(NULL));
				newSpell->SetMass(0.4f + (rand() % 2));
			}
				// after assigning a collider and its role, it add the spell to the game objects list and it decrease the number of magic che player can shoot
			newSpell->SetCollider(new CircleVolume(7.0f));
			newSpell->SetCharacter(SimObject::Character::Spell);
			((FruitWizardGame*)this->game)->AddNewObject(newSpell);
			((FruitWizardGame*)this->game)->SumMagicCount(-1);
		}
	}


	animFrameData = animSource[currentanimFrame];

	return true;
}