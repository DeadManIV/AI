#ifndef i015203eTANK_H
#define i015203eTANK_H

#include "BaseTank.h"
#include <SDL.h>
#include "Commons.h"
#include "Pathfinder.h"

using namespace::std;

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
	
	void UpdateInput(SDL_Event e);

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

	Vector2D ahead;
	Vector2D ahead2;
	double maxSeeAhead = 20.0;
	double maxAvoidForce = 70.0;
	Vector2D combinedForces;

	Pathfinder* pathfinder;
	int currentNode;
	vector<Vector2D> path;
	Vector2D mouseChanged;
};

//---------------------------------------------------------------

#endif //i015203eTANK_H