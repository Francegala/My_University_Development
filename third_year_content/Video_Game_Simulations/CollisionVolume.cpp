#include "CollisionVolume.h"
#include "SimObject.h"
#include "GameSimsRenderer.h"
#include "CircleVolume.h"
#include "AABBVolume.h"

using namespace NCL;
using namespace CSC3222;

    // Constructor saves type of volume passed by inherited classes
CollisionVolume::CollisionVolume(Type t) {
    type = t;
}

/*
    Setter to store a reference to the SimObject when adding a collider through the simobject
    Getter to return the SimObject referred when using a collision volume
*/
void NCL::CSC3222::CollisionVolume::SetSim(SimObject* sObj)
{
    localSO = sObj;
}

SimObject* NCL::CSC3222::CollisionVolume::GetSim()
{
    return this->localSO;

}

/*
    Collision method to check if two circles collide:
    it temporarily saves the position and radii of the two circles
    if the distance between the two centres is less than the sum of the radii they collide
    (distance between two 2D points involve a square root that's why everything is squared)

    If there is a collision, the method saves the information of the collision in both objects calculating the normal and penetration distance
*/
bool NCL::CSC3222::CollisionVolume::CheckCollisionC(CollisionVolume* s2)
{
    //distance between centre less then sum of two radii
    int x1 = s2->GetSim()->GetPosition().x;
    int y1 = s2->GetSim()->GetPosition().y;
    float r1 = ((CircleVolume*)s2)->GetRadius();

    int x2 = localSO->GetPosition().x;
    int y2 = localSO->GetPosition().y;
    float r2 = ((CircleVolume*)this)->GetRadius();

    if ((((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2))) <= ((r1 * r1) + (r2 * r2) + 2 * r1 * r2)) {
        
            //normal = direction in which the object needs to be separated DIRECTION VECTOR BETWEEN TWO OBJECT CENTER NORMAL FACES AWAY FROM A TOWARDS B
            //penetration distance = amount by which two object started to collide SUM RADII MINUS DISTANCE BETWEEN OBJECTS
        
        this->penetrationDistance = (r1 + r2) - (this->GetSim()->GetPosition() - s2->GetSim()->GetPosition()).Length();
        s2->SetPenetrationDistance(this->penetrationDistance);
        this->normal = -(this->GetSim()->GetPosition() - s2->GetSim()->GetPosition()).Normalised();
        s2->SetNormal(this->normal);
        return true;
    }
	return false;

 
}

/*
    Collision method to test two AABB shapes:
    the method calculates the sum of the width and height and the distance between the centres of the two figures
    used to check if a rectangle is inside the other
    for every axes if the distance of centre is less then sum of half sizes on the axis

    It also saves normal and distance if collision occurred to both colliders so they can use those values
*/
bool NCL::CSC3222::CollisionVolume::CheckCollisionS(CollisionVolume* s2)
{ 
    float sumWidth = ((AABBVolume*)this)->GetHalfWidth() + ((AABBVolume*)s2)->GetHalfWidth();
    float sumHeigth = ((AABBVolume*)this)->GetHalfHeight() + ((AABBVolume*)s2)->GetHalfHeight();

    float distanceX = abs(this->GetSim()->GetPosition().x - s2->GetSim()->GetPosition().x);
    float distanceY = abs(this->GetSim()->GetPosition().y- s2->GetSim()->GetPosition().y);

    if (distanceX < sumWidth && distanceY < sumHeigth) {
        
            //normal = direction in which the object needs to be separated AXIS WITH LEAST PENETRATION (sum of half sizes - distance on that axis)
            //penetration distance = amount by which two object started to collide MINIMUM DISTANCE
        
        Vector2 overlap = Vector2(abs(distanceX - sumWidth), abs(distanceY - sumHeigth));
        if (overlap.x < overlap.y)
        {
            this->penetrationDistance = overlap.x;
            s2->SetPenetrationDistance(this->penetrationDistance);
            this->normal = Vector2((s2->GetSim()->GetPosition() - this->GetSim()->GetPosition()).x, 0).Normalised();
            s2->SetNormal(this->normal);

        }
        else {
            this->penetrationDistance = overlap.y;
            s2->SetPenetrationDistance(this->penetrationDistance);
            this->normal = Vector2(0, (s2->GetSim()->GetPosition() - this->GetSim()->GetPosition()).y).Normalised();
            s2->SetNormal(this->normal);
        }
        return true;
    }

    return false;
}

/*
    This method is called from a circle passing as parameter an AABB shape
    1. It determines the closest point between the two shapes
    2. It clamps the sphere position on each axis to have a value between the minimum and maximum extent of the AABB shape
    
    Having the closest point on the AABB to the sphere it is possible to check if the poit lies on a distance less thant the radius from the centre
*/
bool NCL::CSC3222::CollisionVolume::CheckCollisionCS(CollisionVolume* s2) {

    Vector2 closestPoint = Vector2(
                clamp(this->GetSim()->GetPosition().x,
                    (s2->GetSim()->GetPosition().x - ((AABBVolume*)s2)->GetHalfWidth()),
                    (s2->GetSim()->GetPosition().x + ((AABBVolume*)s2)->GetHalfWidth())
                ),
                clamp(this->GetSim()->GetPosition().y,
                    (s2->GetSim()->GetPosition().y - ((AABBVolume*)s2)->GetHalfHeight()),
                    (s2->GetSim()->GetPosition().y + ((AABBVolume*)s2)->GetHalfHeight())
                )
           );

    float distanceSquare = ((closestPoint.x - this->GetSim()->GetPosition().x) * (closestPoint.x - this->GetSim()->GetPosition().x)) +
        ((closestPoint.y - this->GetSim()->GetPosition().y) * (closestPoint.y - this->GetSim()->GetPosition().y));
    
    if (distanceSquare < (((CircleVolume*)this)->GetRadius() * ((CircleVolume*)this)->GetRadius())) {

        //normal is direction vector between closes point and sphere origin
        //pen distance is difference between sphere radius and distance between closest point and origin
        Vector2 normal = closestPoint - this->GetSim()->GetPosition();

        this->SetNormal(normal.Normalised());
        s2->SetNormal(this->getNormal());

        this->SetPenetrationDistance(abs(normal.Length() - ((CircleVolume*)this)->GetRadius()));
        s2->SetPenetrationDistance(this->GetPenetrationDistance());
        return true;
    }
    return false;

}

// Collision checked between an AABB rectangle and a Circle passed as argument: AABB->check collision (Circle)
bool NCL::CSC3222::CollisionVolume::CheckCollisionSC(CollisionVolume* s2) {
    Vector2 closestPoint = Vector2(
        clamp(s2->GetSim()->GetPosition().x,
            (this->GetSim()->GetPosition().x - ((AABBVolume*)this)->GetHalfWidth()),
            (this->GetSim()->GetPosition().x + ((AABBVolume*)this)->GetHalfWidth())
        ),
        clamp(s2->GetSim()->GetPosition().y,
            (this->GetSim()->GetPosition().y - ((AABBVolume*)this)->GetHalfHeight()),
            (this->GetSim()->GetPosition().y + ((AABBVolume*)this)->GetHalfHeight())
        )
    );
    float distanceSquare = ((closestPoint.x - s2->GetSim()->GetPosition().x) * (closestPoint.x - s2->GetSim()->GetPosition().x)) +
        ((closestPoint.y - s2->GetSim()->GetPosition().y) * (closestPoint.y - s2->GetSim()->GetPosition().y));

    if (distanceSquare < (((CircleVolume*)s2)->GetRadius() * ((CircleVolume*)s2)->GetRadius())) {
        Vector2 normal = closestPoint - s2->GetSim()->GetPosition();

        s2->SetNormal(normal.Normalised());
        this->SetNormal(s2->getNormal());

        s2->SetPenetrationDistance(normal.Length() - ((CircleVolume*)s2)->GetRadius());
        this->SetPenetrationDistance(s2->GetPenetrationDistance());
        return true;
    }
    return false;

}

float CollisionVolume::clamp(float value, float minValue, float maxValue) {
    value = min(value, maxValue);
    value = max(value, minValue);
    return value;
}

// method called to display colliders 
void CollisionVolume::DrawCollider()
{
    if (this->getType() == Type::Circle)localSO->GetGame()->GetRenderer()->DrawCircle(this->GetSim()->GetPosition(), ((CircleVolume*)this)->GetRadius(), Vector4(1, 0, 0, 1));
    if (this->getType() == Type::AABB)localSO->GetGame()->GetRenderer()->DrawBox(localSO->GetPosition(), Vector2(((AABBVolume*)this)->GetHalfWidth(),((AABBVolume*)this)->GetHalfHeight()), Vector4(1, 1, 0, 1));
}

CollisionVolume::~CollisionVolume()
{
}