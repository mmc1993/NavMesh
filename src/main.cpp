#include "window/window.h"
#include "window/mesh_window.h"
#include "window/astar_window.h"
#include "astar/astar.h"
#include "../resource.h"

int main()
{
	//MeshWindow meshWindow;
	//meshWindow.Create("Íø¸ñµ¼º½", 100, 100, 800, 400);
	//meshWindow.Loop();

	AStarWindow astarWindow;
	astarWindow.Create("astar", 100, 100, 800, 400);
	astarWindow.Loop();

	//AStar astar;
	//astar.InitFromFile("nav.txt");
	return 0;
}

