#ifndef i015203eTANK_H
#define i015203eTANK_H

#include "BaseTank.h"
#include <SDL.h>
#include "Commons.h"
#include "i015203ePathfinder.h"
#include "PickUpManager.h"

using namespace::std;

enum STATE
{
	COLLECT,
	WANDER,
	FLEE,
	ATTACK_SHOOT,
	ATTACK_MINE,
};
//---------------------------------------------------------------

class i015203eTank : protected BaseTank
{
	//---------------------------------------------------------------
public:
	i015203eTank(SDL_Renderer* renderer, TankSetupDetails details);

	~i015203eTank();

	void ChangeState(BASE_TANK_STATE newState);

	void Update(float deltaTime, SDL_Event e);

	//---------------------------------------------------------------
protected:
	void MoveInHeadingDirection(float deltaTime);

private:
	Vector2D Seek(Vector2D TargetPos);

	Vector2D Flee(Vector2D TargetPos);

	Vector2D Arrive(Vector2D TargetPos, Deceleration deceleration);
	
	Vector2D Pursuit(Vector2D TargetPos);
	
	Vector2D ObstacleAvoidance();

	GameObject* findClosestObstacle();

	float distance(Vector2D a, Vector2D b);

	bool lineIntersectsCircle(Vector2D ahead, Vector2D ahead2, GameObject* obstacle);

	bool lineIntersecsRectangle(Vector2D position, Vector2D ahead, Rect2D r);

	bool isInsideRectangle(Vector2D point, Rect2D r);

	bool Box(Rect2D* character1, Rect2D* character2);

	Vector2D GetClientCursorPosition();

	Vector2D CombinedForces(Vector2D desiredPos);

	void SwitchState(STATE state);

	void CheckIfStateNeedsChange();
	
	void UpdateInput(SDL_Event e);

	void UpdateState(float deltaTime);

	Vector2D mousePos;
	Vector2D mouseMotionPos;
	Vector2D lastMousePos;

	bool seekOn;
	bool fleeOn;
	bool fleeRadiusOn;
	bool arriveOn;
	bool pursuitOn;
	bool obstacleAvoidanceOn;
	bool obstacleCircle;
	bool pathfinderBool;

	//Avoidance
	Vector2D ahead;
	Vector2D ahead2;
	double maxSeeAhead = 20.0;
	double maxAvoidForce = 70.0;
	Vector2D combinedForces;
	
	//States
	STATE currentState;

	//Collect
	Vector2D collectLocation;
	vector<Vector2D> collectPath;
	int collectCurrentNode;
	vector<GameObject*> pickups;

	//Wander
	Vector2D wanderLocation;
	vector<Vector2D> wanderPath;
	int wanderCurrentNode;
	vector<Waypoint*> waypoints;

	//Flee
	Vector2D fleeLocation;
	vector<Vector2D> fleePath;
	int fleeCurrentNode;
	//Tanks in view range can be accessed from the mTanksICanSee variable

	//Attack shoot
	Vector2D lastKnownPos;

	//Pathfinding
	Pathfinder* pathfinder;
	int currentNode;
	vector<Vector2D> path;
	Vector2D mouseChanged;
};

//---------------------------------------------------------------

#endif //i015203eTANK_H