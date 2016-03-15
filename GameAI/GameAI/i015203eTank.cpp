#include "i015203eTank.h"
#include "TankManager.h"
#include "ObstacleManager.h"
#include "Commons.h"
#include "C2DMatrix.h"
#include "ProjectileManager.h"
#include "Collisions.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

i015203eTank::i015203eTank(SDL_Renderer* renderer, TankSetupDetails details)
	: BaseTank(renderer, details)
{
	seekOn = false;
	fleeOn = false;
	fleeRadiusOn = false;
	arriveOn = false;
	pursuitOn = false;
	obstacleAvoidanceOn = true;
	obstacleCircle = false;
	pathfinderBool = false;
	mousePos = Vector2D(0, 0);
	lastMousePos = Vector2D(0, 0);
	mouseChanged = Vector2D(1, 1);
	pathfinder = new Pathfinder();
	currentNode = -1; 
}

//--------------------------------------------------------------------------------------------------

i015203eTank::~i015203eTank()
{

}

//--------------------------------------------------------------------------------------------------

void i015203eTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void i015203eTank::Update(float deltaTime, SDL_Event e)
{
	UpdateInput(e);

	if (mousePos.x != 0 && mousePos.y != 0)
	{	
		if (pathfinderBool)
		{
			if (mouseChanged != mousePos)
			{		
				mouseChanged = mousePos;
				path = pathfinder->FindPath(GetCentralPosition(), mousePos);
				currentNode = path.size()-1;
			}
			if (currentNode == 0)
			{
				seekOn = false;
				arriveOn = true;
			}
			else
			{
				seekOn = true;
				arriveOn = false;
			}

			if (currentNode > -1)
			{
				combinedForces = CombinedForces(path[currentNode]);
			}
			else
			{
				mVelocity = Vector2D(0, 0);
			}	

			if (currentNode > -1 && GetCentralPosition().Distance(mousePos) < GetCentralPosition().Distance(path[currentNode]))
			{
				currentNode = currentNode - 1;
				combinedForces = CombinedForces(mousePos);
			}

			if (currentNode != -1 && isInsideRectangle(path[currentNode], GetAdjustedBoundingBox()))
			{
				currentNode = currentNode - 1;
			}
		}
		else
		{
			combinedForces = CombinedForces(mousePos);
		}
	}

	//Call parent update.
	BaseTank::Update(deltaTime, e);
}

Vector2D i015203eTank::Seek(Vector2D TargetPos)
{
	Vector2D DesiredVelocity = Vec2DNormalize(TargetPos - GetCentralPosition())
		* GetMaxSpeed();

	return (DesiredVelocity - GetVelocity());
}

Vector2D i015203eTank::Flee(Vector2D TargetPos)
{
	//only flee if the target is within 'panic distance'. Work in distance
	//squared space.
	if (fleeRadiusOn)
	{
		const double PanicDistanceSq = 200.0f * 200.0;
		if (Vec2DDistanceSq(GetCentralPosition(), TargetPos) > PanicDistanceSq)
		{
			return Vector2D(0,0);
		}
	}

	Vector2D DesiredVelocity = Vec2DNormalize(GetCentralPosition() - TargetPos)
		* GetMaxSpeed();

	return (DesiredVelocity - GetVelocity());
}

Vector2D i015203eTank::Arrive(Vector2D TargetPos, Deceleration deceleration)
{
	Vector2D ToTarget = TargetPos - GetCentralPosition();

	//calculate the distance to the target
	double dist = ToTarget.Length();
	if (dist < 0.5 && dist > -0.5)
	{
		mVelocity = Vector2D(0, 0);
	}
	else
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double DecelerationTweaker = 1.9f;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = dist / ((double)deceleration * DecelerationTweaker);

		//make sure the velocity does not exceed the max
		speed = min(speed, GetMaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		Vector2D DesiredVelocity = ToTarget * speed / dist;

		return (DesiredVelocity - GetVelocity());
	}

	return Vector2D(0, 0);
}

Vector2D i015203eTank::ObstacleAvoidance()
{
	//float dynamic_length = GetVelocity().Length() / GetMaxSpeed();
	//ahead = GetCentrePosition() + Vec2DNormalize(GetVelocity()) * dynamic_length;

	//ahead = GetCentralPosition() + Vec2DNormalize(GetVelocity()) * maxSeeAhead;
	//ahead2 = GetCentralPosition() + Vec2DNormalize(GetVelocity()) * maxSeeAhead * 0.5;

	//GameObject* mostThreatening = findClosestObstacle();
	//Vector2D avoidance = Vector2D(0, 0);

	//if (mostThreatening != nullptr) 
	//{
	//	avoidance.x = ahead.x - mostThreatening->GetCentralPosition().x;
	//	avoidance.y = ahead.y - mostThreatening->GetCentralPosition().y;

	//	avoidance.Truncate(maxAvoidForce);
	//}
	//else 
	//{
	//	avoidance.Truncate(0);
	//}

	//return avoidance;

	//New type of obstacle avoidance
	Vector2D avoidance = Vector2D(0, 0);

	vector<GameObject*> ObstacleList = ObstacleManager::Instance()->GetObstacles();

	Rect2D tankAdjust = GetAdjustedBoundingBox();

	for (int i = 0; i < ObstacleList.size(); i++)
	{
		if (Box(&ObstacleList[i]->GetAdjustedBoundingBox(), &tankAdjust))
		{
			avoidance = GetCentralPosition() - ObstacleList[i]->GetCentralPosition();
		}
	}

	return avoidance;
}

GameObject* i015203eTank::findClosestObstacle()
{
	GameObject* mostThreatening = nullptr;

	vector<GameObject*> ObstacleList = ObstacleManager::Instance()->GetObstacles();

	for (int i = 0; i < ObstacleList.size(); i++)
	{
		bool collision;
		GameObject* obstacle = ObstacleList[i];
		if (obstacleCircle)
		{
			collision = lineIntersectsCircle(ahead, ahead2, obstacle);
		}
		else
		{
			collision = lineIntersecsRectangle(GetPosition(), ahead, obstacle->GetAdjustedBoundingBox());
			//collision = Collisions::Instance()->PointInBox(ahead, obstacle->GetAdjustedBoundingBox());
		}

		if (collision /*&& distance(mostThreatening->GetCentralPosition(), obstacle->GetCentralPosition())*/)
		{
			mostThreatening = obstacle;
		}
	}

	//ObstacleList = ProjectileManager::Instance()->GetVisibleMines(this);
	//for (int i = 0; i < ObstacleList.size(); i++)
	//{
	//	GameObject* obstacle = ObstacleList[i];
	//	bool collision = lineIntersectsCircle(ahead, ahead2, obstacle);
	//	if (collision && (mostThreatening == nullptr || distance(GetCentralPosition(), obstacle[i].GetCentralPosition()) < distance(GetCentralPosition(), mostThreatening->GetCentralPosition())))
	//	{
	//		mostThreatening = obstacle;
	//	}
	//}

	return mostThreatening;
}

float i015203eTank::distance(Vector2D a, Vector2D b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool i015203eTank::lineIntersectsCircle(Vector2D ahead, Vector2D ahead2, GameObject* obstacle)
{
	return distance(obstacle->GetCentralPosition(), ahead) <= obstacle->GetCollisionRadius() || distance(obstacle->GetCentralPosition(), ahead2) <= obstacle->GetCollisionRadius();
}

bool i015203eTank::lineIntersecsRectangle(Vector2D position, Vector2D ahead, Rect2D r)
{
	return isInsideRectangle(ahead, r) || isInsideRectangle(ahead2, r) || isInsideRectangle(position, r);
}

bool i015203eTank::isInsideRectangle(Vector2D point, Rect2D r)
{
	return point.x >= r.x && point.x <= (r.x + r.width /*+ 15*/) && point.y >= r.y && point.y <= (r.y + r.height /*+ 15*/);
}

bool i015203eTank::Box(Rect2D* character1, Rect2D* character2)
{
	if (character1->x > (character2->x + character2->width))
	return false;
	else if ((character1->x + character1->width) < character2->x)
	return false;
	else if (character1->y > (character2->y + character2->height))
	return false;
	else if ((character1->y + character1->height) < character2->y)
	return false;

	return true;

	return false;
}

Vector2D i015203eTank::Pursuit(Vector2D TargetPos)
{
	if (TargetPos.x == 0 && TargetPos.y == 0)
	{
		lastMousePos = TargetPos;
	}
	Vector2D vecBetween = Vec2DNormalize(TargetPos - lastMousePos);
	lastMousePos = TargetPos;

	Vector2D pursuit = TargetPos;

	if (vecBetween.x != 0 && vecBetween.y != 0)
	{
		pursuit = TargetPos * vecBetween;
	}
	return Seek(pursuit);
}

void i015203eTank::UpdateInput(SDL_Event e)
{
	if (pursuitOn)
	{
		switch (e.type)
		{
		case SDL_MOUSEMOTION:
			mousePos.x = e.motion.x;
			mousePos.y = e.motion.y;
			break;
		}
	}
	else
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			mousePos.x = e.button.x;
			mousePos.y = e.button.y;
			break;
		}
	}
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		//Seek on only
		case SDLK_1:
			if (!seekOn)
			{
				seekOn = true;
			}
			if (fleeOn)
			{
				fleeOn = false;
			}
			if (arriveOn)
			{
				arriveOn = false;
			}
			if (fleeRadiusOn)
			{
				fleeRadiusOn = false;
			}
			if (pursuitOn)
			{
				pursuitOn = false;
			}
			break;
		//Flee on only
		case SDLK_2:
			if (seekOn)
			{
				seekOn = false;
			}
			if (arriveOn)
			{
				arriveOn = false;
			}
			if (!fleeOn)
			{
				fleeOn = true;
			}
			if (fleeRadiusOn)
			{
				fleeRadiusOn = false;
			}
			if (pursuitOn)
			{
				pursuitOn = false;
			}
			break;
		//Flee radius on
		case SDLK_3:
			if (seekOn)
			{
				seekOn = false;
			}
			if (arriveOn)
			{
				arriveOn = false;
			}
			if (!fleeOn)
			{
				fleeOn = true;
			}
			if (!fleeRadiusOn)
			{
				fleeRadiusOn = true;
			}
			if (pursuitOn)
			{
				pursuitOn = false;
			}
			break;
		//Arrive on
		case SDLK_4:
			if (!arriveOn)
			{
				arriveOn = true;
			}
			if (seekOn)
			{
				seekOn = false;
			}
			if (fleeOn)
			{
				fleeOn = false;
			}
			if (fleeRadiusOn)
			{
				fleeRadiusOn = false;
			}
			if (pursuitOn)
			{
				pursuitOn = false;
			}
			break;
		case SDLK_5:
			if (arriveOn)
			{
				arriveOn = false;
			}
			if (seekOn)
			{
				seekOn = false;
			}
			if (fleeOn)
			{
				fleeOn = false;
			}
			if (fleeRadiusOn)
			{
				fleeRadiusOn = false;
			}
			if (!pursuitOn)
			{
				pursuitOn = true;
			}
			break;
		case SDLK_6:
			if (obstacleAvoidanceOn)
			{
				obstacleAvoidanceOn = false;
			}
			else
			{
				obstacleAvoidanceOn = true;
			}
		case SDLK_7:
			if (obstacleCircle)
			{
				obstacleCircle = false;
			}
			else
			{
				obstacleCircle = true;
			}
			break;
		}
	}
}

Vector2D i015203eTank::GetClientCursorPosition()
{
	POINT MousePos;

	GetCursorPos(&MousePos);

	ScreenToClient(GetActiveWindow(), &MousePos);

	return POINTtoVector(MousePos);
}

void i015203eTank::MoveInHeadingDirection(float deltaTime)
{
	//Get the force that propels in current heading.
	Vector2D force = combinedForces;

	//Acceleration = Force/Mass
	Vector2D acceleration = force / GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;

	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.

	//Finally, update the position.
	Vector2D newPosition = GetPosition();
	newPosition.x += mVelocity.x*deltaTime;
	newPosition.y += (mVelocity.y/**-1.0f*/)*deltaTime;	//Y flipped as adding to Y moves down screen.
	SetPosition(newPosition);

	//Rotate to face heading
	Vector2D ahead = mVelocity;
	if (ahead.x == 0.0f && ahead.y == 0.0f)
		ahead = mHeading;
	Vec2DNormalize(ahead);

	RotateHeadingToFacePosition(GetCentralPosition() + ahead * 10.0f);
}

Vector2D i015203eTank::CombinedForces(Vector2D desiredPos)
{
	Vector2D accumulatedForce = Vector2D(0,0);

	if (seekOn)
	{
		accumulatedForce += /*0.2 **/ Seek(desiredPos)/* * 3*/;
	}

	if (fleeOn)
	{
		accumulatedForce += Flee(desiredPos);
	}

	if (arriveOn)
	{
		accumulatedForce += Arrive(desiredPos, normal) * 3;
	}

	if (pursuitOn)
	{
		accumulatedForce += Pursuit(desiredPos);
	}
	
	if (obstacleAvoidanceOn)
	{
		accumulatedForce += /*0.8 **/ ObstacleAvoidance();
	}

	return accumulatedForce;
}

//--------------------------------------------------------------------------------------------------