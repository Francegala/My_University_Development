#include "Fruit.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"

using namespace NCL;
using namespace CSC3222;

	//	The constructor automatically adds a collider to the object and defines the type of Character
Fruit::Fruit() : SimObject() {
	texture = texManager->GetTexture("FruitWizard\\Fruit.png");
	this->SetCollider(new CircleVolume(8.0f));
	this->SetCharacter(SimObject::Character::Fruit);
	int fruitID = rand() % 16;
	animFrameData = Vector4((fruitID / 4) * 16.0f, (fruitID % 4) * 16.0f, 16.0f, 16.0f);
}

Fruit::~Fruit() {
}

	//	If the objects needs to be deleted from the game a boolean gets set
bool Fruit::UpdateObject(float dt) {
	if (this->erased)return false;
	return true;
}