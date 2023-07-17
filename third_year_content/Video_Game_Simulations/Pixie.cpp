#include "Pixie.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 pixieFrames[] = {
	Vector4(32 * 1,32 * 4, 32, 32),
	Vector4(32 * 2,32 * 4, 32, 32),
	Vector4(32 * 3,32 * 4, 32, 32),
	Vector4(32 * 4,32 * 4, 32, 32),
};
	// The collider saves a reference to the Player Character so when calling the Spring function in Update, even if the other connection of the spring might have changed due to another collision, it always has a reference to it
Pixie::Pixie(PlayerCharacter* p) : SimObject() {
	texture			= texManager->GetTexture("FruitWizard\\super_random_sprites.png");
	animFrameCount	= 4;
	this->SetCollider(new CircleVolume(8.0f));
	this->SetCharacter(SimObject::Character::Pixie);
	this->playerReference = p;
}

Pixie::~Pixie() {
}

	// Spring method called with the player character reference as parameter in order to have it always conected to the main character
bool Pixie::UpdateObject(float dt) {
	if (this->erased)return false;
	animFrameData = pixieFrames[currentanimFrame];
	this->Spring(this->playerReference);
	return true;
}