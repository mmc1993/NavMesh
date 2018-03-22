#pragma once

#include "../window/window.h"
#include "../astar/astar.h"

class AStarWindow : public Window {
public:
	AStarWindow();
	~AStarWindow();

private:
	virtual void OnMouse(u_int msg, int x, int y, u_int key, int wheel) override;
	virtual void OnKey(u_int msg, int flag, int key) override;
	virtual void OnResize(int w, int h) override;
	virtual void OnPaint() override;
	
	bool InitBrush();

private:
	AStar _astar;
	math::Vec2 _startpt;
	std::vector<math::Vec2> _waypoints;
	std::vector<std::uint16_t> _navmesh;

private:
	ID2D1SolidColorBrush * _pDxBrush;
};