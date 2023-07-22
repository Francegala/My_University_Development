#include "PixieDust.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 animFrames[] = {
	Vector4(64 , 192, 32, 32),
	Vector4(96, 192, 32, 32),
	Vector4(128, 192, 32, 32),
	Vector4(160, 192, 32, 32)
};

	// Constructor with collider attached and type of character
PixieDust::PixieDust() : SimObject() {
	animFrameCount = 4;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	this->SetCollider(new CircleVolume(10.0f));
	this->SetCharacter(SimObject::Character::PixieDust);
}

PixieDust::~PixieDust() {
}

	// This object can be destroyed by setting the flag
bool PixieDust::UpdateObject(float dt) {
	animFrameData = animFrames[currentanimFrame];
	if (this->erased)return false;
	return true;
}