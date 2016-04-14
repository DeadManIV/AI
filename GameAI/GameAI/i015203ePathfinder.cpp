#include "i015203ePathfinder.h"


Pathfinder::Pathfinder()
{

}


Pathfinder::~Pathfinder()
{
}

vector<Vector2D> Pathfinder::FindPath(Vector2D startPos, Vector2D goalPos)
{
	openList.clear();
	closedList.clear();
	path.clear();

	startNode = new Node();
	goalNode = new Node();

	startNode->waypoint = FindClosestWaypoint(startPos);
	goalNode->waypoint = FindClosestWaypoint(goalPos);

	startNode->g = 0; //Start point so no cost
	startNode->h = startNode->waypoint->GetPosition().Distance(goalNode->waypoint->GetPosition());
	startNode->f = startNode->g + startNode->h;
	startNode->parent = nullptr;

	openList.push_back(startNode);

	while (openList.size() > 0)
	{
		int lowestF = 0;
		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i]->f < openList[lowestF]->f) 
			{ 
				lowestF = i; 
			}
		}

		Node* currentNode = openList[lowestF];

		//Found it
		if (currentNode->waypoint == goalNode->waypoint) 
		{
			Node* node = currentNode;
			path.push_back(goalPos);

			while (node->parent != nullptr)
			{
				path.push_back(node->waypoint->GetPosition());
				node = node->parent;
			}
			path.push_back(node->waypoint->GetPosition());
			return path;
		}
		
		openList.erase(openList.begin() + lowestF);
		closedList.push_back(currentNode);

		vector<int>	children = currentNode->waypoint->GetConnectedWaypointIDs();

		for (int i = 0; i < children.size(); i++)
		{		
			//Set up child (connected waypoint)
			Node* child = new Node();
			child->waypoint = WaypointManager::Instance()->GetWaypointWithID(children[i]);
			child->g = currentNode->g + currentNode->waypoint->GetPosition().Distance(child->waypoint->GetPosition()); 
			child->h = child->waypoint->GetPosition().Distance(goalNode->waypoint->GetPosition());
			child->f = child->g + child->h;
			
			bool onOpenList = false;
			
			//If on closed list skip to next child
			for (int i = 0; i < closedList.size(); i++)
			{
				if (child->waypoint == closedList[i]->waypoint)
				{
					continue;
				}
			}

			//Check if its on the open list
			for (int i = 0; i < openList.size(); i++)
			{
				if (child->waypoint == openList[i]->waypoint)
				{
					onOpenList = true;
				}
			}

			//If not on open list add to open list and if child has better f set parent
			if (!onOpenList)
			{
				openList.push_back(child);
				child->parent = currentNode;
			}
		}
	}
}

float Pathfinder::GetDistanceBetween(Vector2D first, Vector2D second)
{
	float x = first.x - second.x;
	float y = first.y - second.y;
	return x + y;
}

Waypoint* Pathfinder::FindClosestWaypoint(Vector2D position)
{
	vector<Waypoint*> waypoints = WaypointManager::Instance()->GetAllWaypoints();
	Waypoint* closestWaypoint = waypoints[0];
	for (int i = 1; i < waypoints.size(); i++)
	{	
		if (position.Distance(waypoints[i]->GetPosition()) < position.Distance(closestWaypoint->GetPosition()))
		{
			closestWaypoint = waypoints[i];
		}
	}
	return closestWaypoint;
}