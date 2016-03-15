#ifndef Pathfinder_H
#define Pathfinder_H

#include <SDL.h>
//#include "Commons.h"
#include "WaypointManager.h"
#include "Waypoint.h"

using namespace::std;

struct Node
{
	Node* parent;
	Waypoint* waypoint;
	float f;
	float g;
	float h;
};

class Pathfinder
{
public:
	Pathfinder();

	~Pathfinder();

	vector<Vector2D> FindPath(Vector2D startPos, Vector2D goalPos);

	float GetDistanceBetween(Vector2D first, Vector2D second);

	Waypoint* FindClosestWaypoint(Vector2D pos);

private:
	Node* startNode;
	Node* goalNode;
	vector<Node*> openList;
	vector<Node*> closedList;
	vector<Vector2D> path;
};
#endif //Pathfinder_H
