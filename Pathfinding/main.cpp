#include <iostream>
#include <vector>
using namespace std;

#include <windows.h>

#include "Map.h"

int PlayerPositionX		= 5;
int PlayerPositionY		= 8;

int TargetPositionX		= 5;
int TargetPositionY		= 3;

struct PathfindNode
{
public:
	int				X;
	int				Y;
	PathfindNode*	Parent;

	PathfindNode(int x, int y, PathfindNode* parent = NULL) : X(x), Y(y), Parent(parent) {}
};

void RenderMap( void );
bool PathfindToLocation();
bool IsWithinBounds( int X, int Y );
bool IsNotBoundary( int X, int Y );
bool NodeExists( int X, int Y );
void AddNodeIfAble( int X, int Y, PathfindNode* parent, PathfindNode*& complete_path);

// Pathfinding lists
vector<PathfindNode*> OpenList;
vector<PathfindNode*> NextList;
vector<PathfindNode*> ClosedList;

int main(int argc, char* argv[])
{
	//  Place the player
	Map[PlayerPositionY][PlayerPositionX] = 'X';

	//  Place some random blocks to move around
	Map[5][1] = 'B';
	Map[5][2] = 'B';
	Map[5][3] = 'B';
	Map[5][4] = 'B';
	Map[5][5] = 'B';
	Map[5][6] = 'B';
	Map[5][7] = 'B';
	Map[5][8] = 'B';
	Map[5][9] = 'B';
	//Map[5][10] = 'B';

	//  Pathfind around the blocks
	PathfindToLocation();

	RenderMap();

	cout << endl << endl;
	return 0;
}

void RenderMap( void )
{
	for (unsigned int i = 0; i < MAP_ROWS; ++i) cout << Map[i] << endl;
}


//  Some notes:
//		- This pathfinding algorithm will run an infinite loop if you specify a location it can't get to
//
//		It's just an example

bool PathfindToLocation()
{
	PathfindNode* root_node = new PathfindNode(PlayerPositionX, PlayerPositionY);
	OpenList.push_back(root_node);

	PathfindNode* finished_path = 0;
	while (finished_path == 0)
	{
		for (unsigned int i = 0; i < NextList.size(); ++i) OpenList.push_back(NextList[i]);
		NextList.clear();

		for (unsigned int i = 0; i < OpenList.size(); ++i)
		{
			// Look Up
			AddNodeIfAble(OpenList[i]->X, OpenList[i]->Y - 1, OpenList[i], finished_path);

			// Look Left
			AddNodeIfAble(OpenList[i]->X - 1, OpenList[i]->Y, OpenList[i], finished_path);

			// Look Right
			AddNodeIfAble(OpenList[i]->X + 1, OpenList[i]->Y, OpenList[i], finished_path);

			// Look Down
			AddNodeIfAble(OpenList[i]->X, OpenList[i]->Y + 1, OpenList[i], finished_path);
		}

		if (OpenList.empty()) break;
		for (unsigned int i = 0; i < OpenList.size(); ++i) ClosedList.push_back(OpenList[i]);
		OpenList.clear();
	}

	if (finished_path)
	{
		Map[finished_path->Y][finished_path->X] = 'P';
		while (finished_path = finished_path->Parent) Map[finished_path->Y][finished_path->X] = '.';
	}

	//  Erase all data
	for (unsigned int i = 0; i < NextList.size(); ++i) delete NextList[i];
	NextList.clear();
	for (unsigned int i = 0; i < OpenList.size(); ++i) delete OpenList[i];
	OpenList.clear();
	for (unsigned int i = 0; i < ClosedList.size(); ++i) delete ClosedList[i];
	ClosedList.clear();

	return (finished_path != 0);
}

bool IsWithinBounds( int X, int Y )
{
	if (X < 0)			return false;
	if (X >= MAP_COLS)	return false;
	if (Y < 0)			return false;
	if (Y >= MAP_ROWS)	return false;

	return true;
}

bool IsNotBoundary( int X, int Y )
{
	if (Map[Y][X] == '#') return false;
	if (Map[Y][X] == 'B') return false;

	return true;
}

bool NodeExists( int x, int y )
{
	//  Check to see if the location is already in the closed or open list
	for (unsigned int i = 0; i < OpenList.size(); ++i)
		if (OpenList[i]->X == x && OpenList[i]->Y == y)
			return true;

	for (unsigned int i = 0; i < ClosedList.size(); ++i)
		if (ClosedList[i]->X == x && ClosedList[i]->Y == y)
			return true;

	for (unsigned int i = 0; i < NextList.size(); ++i)
		if (NextList[i]->X == x && NextList[i]->Y == y)
			return true;

	return false;
}

void AddNodeIfAble( int x, int y, PathfindNode* parent, PathfindNode*& complete_path )
{
	if (IsWithinBounds(x, y) && IsNotBoundary(x, y) && !NodeExists(x, y))
	{
		PathfindNode* new_node = new PathfindNode(x, y, parent);
		NextList.push_back(new_node);
		if (x == TargetPositionX && y == TargetPositionY) complete_path = new_node;
	}
}