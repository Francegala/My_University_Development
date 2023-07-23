#include "SimObject.h"
#include "CollisionVolume.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

TextureManager*  SimObject::texManager	= nullptr;
FruitWizardGame* SimObject::game		= nullptr;

SimObject::SimObject()	{
	texture		= nullptr;
	collider	= nullptr;

	animFrameCount		= 1;
	currentanimFrame	= 0;
	frameTime			= 0;
	frameRate			= 10;
	flipAnimFrame		= true;
}

SimObject::~SimObject()	{
	delete collider;
}

		// It both attach a collider to the SimObject but it also reference the SimObject to the collider
void NCL::CSC3222::SimObject::SetCollider(CollisionVolume* c)
{
		c->SetSim(this);
		collider = c;
}

void SimObject::InitObjects(FruitWizardGame* game, TextureManager* texManager) {
	SimObject::game			= game;
	SimObject::texManager	= texManager;
}

//	method used to display in console what object is selected
void SimObject::PrintEnum()
{
	if (this->GetCharacter() == SimObject::Character::Player) std::cout << "Player";
	if (this->GetCharacter() == SimObject::Character::Froggo) std::cout << "Froggo";
	if (this->GetCharacter() == SimObject::Character::Fruit) std::cout << "Fruit";
	if (this->GetCharacter() == SimObject::Character::Guard) std::cout << "Guard";
	if (this->GetCharacter() == SimObject::Character::Wall) std::cout << "Wall";
	if (this->GetCharacter() == SimObject::Character::Pixie) std::cout << "Pixie";
	if (this->GetCharacter() == SimObject::Character::PixieDust) std::cout << "PixieDust";
	if (this->GetCharacter() == SimObject::Character::Spell) std::cout << "Spell";
}

/*
	If the object has no another SimObject as peer or a different one, it gets added/updated
	It uses 0.6 as snappiness to have a soft spring and 1 as dumping constant as friction
	The resting lenght is the length of the spring when "not used"
	The distance represent how much stretched or compressed the spring is and for the direction is almost the same calculation as the stretch lenght, 
	difference between position of the two object at the start and end of the spring as lenght minus the resting lenght or normalised for the direction
	The forces is as the formula -kx and the relative velocity calculated as the dot product between the direction of the spring and the difference between the velcoities of the two objects
	the force is added to only one edge of the spring subtracting the relative velocity multiplied by the dumping factor
*/
void NCL::CSC3222::SimObject::Spring(SimObject* pair)
{

	if (this->springPair != pair)this->springPair = pair;
	float c = 1;		// dumping constant
	float k = 0.6f;		// snappiness
	int restingLength = 18;

	float distance = ((this->GetPosition() - this->springPair->GetPosition()).Length()) - restingLength;
	Vector2 springDirection = (this->GetPosition() - this->springPair->GetPosition()).Normalised();
	float force = -1 * k * distance; // F = -kx

	float velocitySpringAxis = Vector2::Dot(this->GetVelocity() - this->GetSpringPair()->GetVelocity(), springDirection);
	this->AddForce(springDirection * (force - (c * velocitySpringAxis)));

}

//Each Boid looks at all of the nearby Boids within a threshold distance and applies an amount of force in their average direction
Vector3 SimObject::Alignment(std::vector<SimObject*> allBoids)
{
	Vector3 dir = this->GetVelocity();
	for (int b = 0; b < allBoids.size(); b++) {
		if (allBoids[b] == this) {
			continue;
		}
		float distance = (this->GetPosition() - allBoids[b]->GetPosition()).Length();
		if (distance > 150) {
			continue;
		}
		dir += allBoids[b]->GetVelocity();
	}
	return dir.Normalised();
}

// Boids move avay from their neighbours such that collisions can't happpen.
Vector3 NCL::CSC3222::SimObject::Separation(std::vector<SimObject*> allBoids)
{
	Vector3 dir;
	for (int b = 0; b < allBoids.size(); b++) {
		if (allBoids[b] == this) {
			continue;
		}
		float distance = (this->GetPosition() - allBoids[b]->GetPosition()).Length();
		if (distance > 150) {
			continue;
		}
		float strenth = 1.5f - (distance / 150);
		dir += (this->GetPosition() - allBoids[b]->GetPosition()).Normalised() * strenth;
	}
	return dir; // here I am not normalising otherwise the change was too little
}

// Boids all apply forces towards the average position of their neighbours
Vector3 NCL::CSC3222::SimObject::Cohesion(std::vector<SimObject*> allBoids)
{
	Vector3 averagePosition = this->GetPosition();
	float count = 1;

	for (int b = 0; b < allBoids.size(); b++) {
		if (allBoids[b] == this) {
			continue;
		}
		float distance = (this->GetPosition() - allBoids[b]->GetPosition()).Length();
		if (distance > 150) {
			continue;
		}
		averagePosition += allBoids[b]->GetPosition();
		count++;
	}
	averagePosition /= count;
	Vector3 dir = averagePosition - this->GetPosition();
	return dir.Normalised();
}

// members work out the mean direction away from any nearby enemy Bouds abd apply forces in that direction to avoid them
// I am passing directly a list of enemies so the member of Boids calling the function will avoid all of them
Vector3 NCL::CSC3222::SimObject::Avoidance(std::vector<SimObject*> allBoidsEnemies)
{
	Vector3 dir;
	for (int b = 0; b < allBoidsEnemies.size(); b++) {
		float distance = (this->GetPosition() - allBoidsEnemies[b]->GetPosition()).Length();
		if (distance > 250) { // include as many guards as possible to avoid
			continue;
		}
		dir += (this->GetPosition() - allBoidsEnemies[b]->GetPosition()).Normalised();
		
	}
	return dir.Normalised();
}

void SimObject::UpdateAnimFrame(float dt) {
	frameTime -= dt;

	if (frameTime < 0) {
		currentanimFrame = (currentanimFrame + 1) % animFrameCount;
		frameTime += 1.0f / frameRate;
	}
}

void SimObject::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos		= Vector2(animFrameData.x, animFrameData.y);
	Vector2 texSize		= Vector2(animFrameData.z, animFrameData.w);
	Vector2 screenPos	= position;

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, flipAnimFrame);
}