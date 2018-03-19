#include "window/window.h"
#include "editor/mesh_window.h"
#include "editor/astar_window.h"
#include "astar/astar.h"
#include "../resource.h"

BOOL CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
{
	return FALSE;
}

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

