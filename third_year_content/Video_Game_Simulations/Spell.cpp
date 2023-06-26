#include "Spell.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace CSC3222;

Vector4 activeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
};

Vector4 explodeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(416,416, 32, 32),
	Vector4(448,416, 32, 32),
	Vector4(480,416, 32, 32),
	Vector4(512,416, 32, 32),	
};

Spell::Spell(Vector2 direction) : SimObject()	{
	texture		= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	velocity	= direction;
	animFrameCount = 6;
}

Spell::~Spell()	{
}

void Spell::DrawObject(GameSimsRenderer &r) {
	Vector4	texData = explodeFrames[currentanimFrame];
	Vector2 texPos	= Vector2(texData.x, texData.y);
	Vector2 texSize = Vector2(texData.z, texData.w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

bool Spell::UpdateObject(float dt) {
/*
	conditions to have the spell deleted:
		- 3 bounces
		- flag erased set in case for example it collides with an enemy
		- the velocity is zero
*/
	if (
		(this->bounces <= 0) ||
		(this->erased) ||	
		 (this->velocity.x == 0 && this->velocity.y == 0)
		)	return false;

/*
	To keep the spell moving (diagonal) at every update the velocity gets set again and again
	to 35 or -35 depending on the direction of the spell which is decided when creating it
	in player character class adding a beginning force
*/
	//constant velocity = no adding force but change velocity and bounce -> change sign to change direction
	if (this->velocity.x < 0) {
		this->velocity.x=-35;
	}
	else {
		this->velocity.x=35;
	}
	if (this->velocity.y < 0) {
		this->velocity.y = -35;
	}
	else {
		this->velocity.y = 35;
	}
	animFrameData = explodeFrames[currentanimFrame];	
	return true;
}