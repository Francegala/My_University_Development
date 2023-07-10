#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include <map>
#include <iterator>
#include "CircleVolume.h"

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics()	{

}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {
	Integration(dt);
		/*
			after integrating accelleration it checks for collision calling broad phase, 
			then if there are possible collisions it calls inside the narrow phase and if there are real collision the resolution
			at the end the velocity gets integrated
		*/
	BroadPhase();
	IntegrateVelocity(dt);
}

		// update function called with a fixed delta time from tutorial physics 3
void GameSimsPhysics::FixedPhysics(float realDT){
	timeRemaining += realDT;
	const float subTimeDelta = 1.0 /120;
	while (timeRemaining > subTimeDelta){
		Update(subTimeDelta);
		timeRemaining -= subTimeDelta;
	}
}
void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

		// rather than having a max velocity, I'm using a dumping factor at accel. integration
void GameSimsPhysics::Integration(float dt) {
	for (int i = 0; i < allBodies.size(); i++) {
		Vector2 accelleration = allBodies[i]->force * allBodies[i]->inverseMass;
		allBodies[i]->velocity *= 0.99f;
		allBodies[i]->velocity += accelleration * dt;
	}
}

		// rather than having a max velocity, using another dumping factor at veloc. integration too
void NCL::CSC3222::GameSimsPhysics::IntegrateVelocity(float dt)
{
	for (int i = 0; i < allBodies.size(); i++) {
		allBodies[i]->position += allBodies[i]->velocity * dt;
		allBodies[i]->velocity *= 0.98f;
		allBodies[i]->force = Vector2(0, 0);
	}
}

/*
		Broad phase, first part of collision detection:
			Iterting through the list of all colliders, every object gets inserted into a map allowing duplicicates since the key used to order them is the position plus radius or height on the y axis
			In this way the function has an sorted list with obects in ascending order on the y axis

			After having the map in order, we don't need to keep the key->value pairs so we transfer only the values in order inside a vector
			and iterating one by one we check if there might be possible collisions, if so the groups are added into a list

			A list containing these little lists of objects is passed to the narrow phase
*/
void NCL::CSC3222::GameSimsPhysics::BroadPhase()
{
	// clear previous stored vectors
	std::vector<CollisionVolume*>	orderedColliders;
	std::vector<std::vector<CollisionVolume*>>	pairsColliders;

	// create a map to order a vector according to x axis position
	std::multimap <float, CollisionVolume*> sortedAxis;
	for (int i = 0; i < allColliders.size(); ++i) {
		if (allColliders[i]->getType() == CollisionVolume::Type::Circle) {
			float objPosX = (allColliders[i]->GetSim()->GetPosition().y - ((CircleVolume*)allColliders[i])->GetRadius());
			sortedAxis.insert(std::pair<float, CollisionVolume*>(objPosX, allColliders[i]));
		}
		else {
			float objPosX = (allColliders[i]->GetSim()->GetPosition().y - ((AABBVolume*)allColliders[i])->GetHalfHeight());
			sortedAxis.insert(std::pair<float, CollisionVolume*>(objPosX, allColliders[i]));
		}
	}
	//transfer ordered map into a vector
		std::multimap <float, CollisionVolume* >::iterator itr;
		for (itr = sortedAxis.begin(); itr != sortedAxis.end(); ++itr) {
			orderedColliders.push_back(itr->second);
		}

		//iterate the vector and if object min y is before anoth object max y they might collide so insert into a list
		for (int i = 0; i < orderedColliders.size() -1; ++i) {
			float maxPos;
			std::vector<CollisionVolume*>pair;
			pair.push_back(orderedColliders[i]);
			if (orderedColliders[i]->getType() == CollisionVolume::Type::Circle) {
				// y rather than x axis because the ground tile at the far bottom covers the whole x axis so every frame we would have 72 comparisons
				maxPos = (orderedColliders[i]->GetSim()->GetPosition().y + ((CircleVolume*)orderedColliders[i])->GetRadius());
			}
			else {
				maxPos = (orderedColliders[i]->GetSim()->GetPosition().y + ((AABBVolume*)orderedColliders[i])->GetHalfHeight());
			}
			for (int j = i+1; j < orderedColliders.size(); ++j) {

				if (orderedColliders[j]->getType() == CollisionVolume::Type::Circle) {
					if (orderedColliders[j]->GetSim()->GetPosition().y - ((CircleVolume*)orderedColliders[j])->GetRadius() < maxPos) {
						pair.push_back(orderedColliders[j]);
					}
				}else{

					if (orderedColliders[j]->GetSim()->GetPosition().y - ((AABBVolume*)orderedColliders[j])->GetHalfHeight() < maxPos) {
						pair.push_back(orderedColliders[j]);

					}
				}
			}
			// if there is a list of possible collision store it
			if (pair.size()>1)pairsColliders.push_back(pair);
		}
		NarrowPhase(pairsColliders);//and resolution if applicable with impulse

}

/*
	Narrow Phase:
		it receives a list containing lists of possible collisions
		and iterating the elements inside those little lists we check if there is a collision

			First pairs of elements with no speed at all gets discarded since they might be placed randomly in the map and since they are still,
			it is likely that they overlap but not resolution is necessary like two fruit spawned close or two pixie dust

			Then also objects that we know being static or that a collision between them is not needed gets discarded from the collision list
			e.g. wall tile with another wall tile or the pixie that roams looking for the player but does not need to collide with anything that is not the player
				or fruit/pixie dust that collides only with the player as well or enemies that might run one into another but no need to resolve the collision (maybe for now)

		When we have two objects that might collide, the fucntion check if the collider shape is a AABB/rectangle-square or a circle
		and calling the spicific function according to order and shape if they collide it first pass the two object to a function in FruitWizardGame class
		which manage game "effects", "rules" that make things happen like a guard to be stunned or the player to win or lose life

		and then it calls the collision resolution to separate the object colliding
*/
void GameSimsPhysics::NarrowPhase(std::vector<std::vector<CollisionVolume*>>	pairsColliders) {
	if (!pairsColliders.empty()) { //if some objects might collide
		for (int k = 0; k < pairsColliders.size(); ++k) { // iterate through the list of lists
			std::vector<CollisionVolume*>	temp = pairsColliders[k]; // grab one by one
			for (int i = 0; i < temp.size() - 1; ++i) {
				for (int j = i + 1; j < temp.size(); ++j)
				{
					if (temp[i]->GetSim()->GetVelocity() == Vector2(0, 0) && temp[j]->GetSim()->GetVelocity() == Vector2(0, 0)) continue; //avoid sleeping objects

					//list of static objects rather than having a separate list or a field, I'm skipping them here
					if (

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Wall &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Wall) ||

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Guard &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Guard) ||
						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Froggo &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Froggo) ||

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Guard &&
								temp[j]->GetSim()->GetCharacter() == SimObject::Character::Froggo) ||
						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Froggo &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Guard) ||

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Pixie &&
							temp[j]->GetSim()->GetCharacter() != SimObject::Character::Player) ||
						(temp[i]->GetSim()->GetCharacter() != SimObject::Character::Player &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Pixie)||

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::Fruit &&
							temp[j]->GetSim()->GetCharacter() != SimObject::Character::Player) ||
						(temp[i]->GetSim()->GetCharacter() != SimObject::Character::Player &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::Fruit)||

						(temp[i]->GetSim()->GetCharacter() == SimObject::Character::PixieDust &&
							temp[j]->GetSim()->GetCharacter() != SimObject::Character::Player) ||
						(temp[i]->GetSim()->GetCharacter() != SimObject::Character::Player &&
							temp[j]->GetSim()->GetCharacter() == SimObject::Character::PixieDust)
						)
					{
						continue;
					}

					if (temp[i]->getType() == CollisionVolume::Type::AABB && temp[j]->getType() == CollisionVolume::Type::AABB) {
						if (temp[i]->CheckCollisionS(temp[j]) == true)
						{
							temp[i]->GetSim()->PrintEnum();
							std::cout << " collided (AABB-AABB) with ";
							temp[j]->GetSim()->PrintEnum();
							std::cout << "\n";
							((FruitWizardGame*)temp[i]->GetSim()->GetGame())->CollisionGame(temp[i]->GetSim(), temp[j]->GetSim());
							CollisionResolution(temp[i], temp[j]);
							break;
						}
					}
					else if (temp[i]->getType() == CollisionVolume::Type::Circle && temp[j]->getType() == CollisionVolume::Type::Circle) {
						if (temp[i]->CheckCollisionC(temp[j]) == true)
						{
							temp[i]->GetSim()->PrintEnum();
							std::cout << " collided (Circle-Circle with ";
							temp[j]->GetSim()->PrintEnum();
							std::cout << "\n";
							((FruitWizardGame*)temp[i]->GetSim()->GetGame())->CollisionGame(temp[i]->GetSim(), temp[j]->GetSim());
							CollisionResolution(temp[i], temp[j]);
							break;
						}
					}
					else if (temp[i]->getType() == CollisionVolume::Type::AABB && temp[j]->getType() == CollisionVolume::Type::Circle) {
						if (temp[i]->CheckCollisionSC(temp[j]) == true)
						{
							temp[i]->GetSim()->PrintEnum();
							std::cout << " collided (AABB-Circle) with ";
							temp[j]->GetSim()->PrintEnum();
							std::cout << "\n";
							((FruitWizardGame*)temp[i]->GetSim()->GetGame())->CollisionGame(temp[i]->GetSim(), temp[j]->GetSim());
							CollisionResolution(temp[i], temp[j]);
							break;
						}
					}
					else if (temp[i]->getType() == CollisionVolume::Type::Circle && temp[j]->getType() == CollisionVolume::Type::AABB) {
						if (temp[i]->CheckCollisionCS(temp[j]) == true)
						{
							temp[i]->GetSim()->PrintEnum();
							std::cout << " collided (Circle-AABB) with ";
							temp[j]->GetSim()->PrintEnum();
							std::cout << "\n";
							((FruitWizardGame*)temp[i]->GetSim()->GetGame())->CollisionGame(temp[i]->GetSim(), temp[j]->GetSim());
							CollisionResolution(temp[i], temp[j]);
							break;
						}
					}
				}
			}
		}
	}
}

/*
	Collision Resolution: final step in collision:
		it receives from narrow phase two colliders of objects overlapping
		1. Positions of both objects get changed to be sure they do not overlap anymore using penetration distance and normal
		2. Velocity of both objects receive an "impulse" so objects keeps moving to stay distant one another
		3. They act as if connected by a spring to avoid running one into the other over and over again
*/
void NCL::CSC3222::GameSimsPhysics::CollisionResolution(CollisionVolume* cV1, CollisionVolume* cV2)
{
	float sumMasses = cV1->GetSim()->GetInverseMass() + cV2->GetSim()->GetInverseMass();

	cV1->GetSim()->SetPosition(cV1->GetSim()->GetPosition() - (
		cV1->getNormal() * cV1->GetPenetrationDistance() * (cV1->GetSim()->GetInverseMass() / sumMasses)
		));

	cV2->GetSim()->SetPosition(cV2->GetSim()->GetPosition() + (
		cV2->getNormal() * cV2->GetPenetrationDistance() * (cV2->GetSim()->GetInverseMass() / sumMasses)
		));

	// how much object velocities are moving into the collision
	// dot product of relative velocity and normal
	float avgCoefficentRestitution = (cV1->GetSim()->GetElasticity() + cV2->GetSim()->GetElasticity())/2;
	Vector2 relativeVelocities = cV2->GetSim()->GetVelocity() - cV1->GetSim()->GetVelocity();
	float velocitiesIntoCollision = Vector2().Dot(relativeVelocities, cV2->getNormal());
	float j = ((-1) * (1 + avgCoefficentRestitution) * (velocitiesIntoCollision)) / sumMasses;
	// no need to apply impulse it wouldn't move anyway!
	if (cV1->GetSim()->GetInverseMass() != 0)cV1->GetSim()->SetVelocity(cV1->GetSim()->GetVelocity() - (cV1->getNormal() * cV1->GetSim()->GetInverseMass() * j));
	if (cV2->GetSim()->GetInverseMass() != 0)cV2->GetSim()->SetVelocity(cV2->GetSim()->GetVelocity() + (cV2->getNormal() * cV2->GetSim()->GetInverseMass() * j));

	cV1->GetSim()->Spring(cV2->GetSim());
}