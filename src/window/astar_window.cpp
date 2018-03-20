#include "astar_window.h"

AStarWindow::AStarWindow()
{
}

AStarWindow::~AStarWindow()
{
}

void AStarWindow::OnMouse(u_int msg, int x, int y, u_int key, int wheel)
{
	if (WM_LBUTTONUP == msg)
	{
		_startpt.x = (float)x;
		_startpt.y = (float)y;
		Log("start: {0}", _astar.GetMeshID({ (float)x, (float)y }));
	}
	else if (WM_RBUTTONUP == msg)
	{
		Log("end: {0}", _astar.GetMeshID({ (float)x, (float)y }));
		_waypoints.clear();
		_astar.GetPath(_astar.GetMeshID(_startpt), _startpt,
					   _astar.GetMeshID({ (float)x, (float)y }), { (float)x, (float)y }, _waypoints);
	}
}

void AStarWindow::OnKey(u_int msg, int flag, int key)
{
	if (msg == WM_KEYUP)
	{
		switch (key)
		{
		case VK_RETURN: _astar.InitFromFile("nav.txt"); break;
		}
	}
}

void AStarWindow::OnResize(int w, int h)
{
}

void AStarWindow::OnPaint()
{
	if (InitBrush())
	{
		_pDxBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		for (auto & mesh : _astar.GetMeshs())
		{
			GetRT()->DrawLine(D2D1::Point2F(mesh.tri.pt1.x, mesh.tri.pt1.y), D2D1::Point2F(mesh.tri.pt2.x, mesh.tri.pt2.y), _pDxBrush);
			GetRT()->DrawLine(D2D1::Point2F(mesh.tri.pt2.x, mesh.tri.pt2.y), D2D1::Point2F(mesh.tri.pt3.x, mesh.tri.pt3.y), _pDxBrush);
			GetRT()->DrawLine(D2D1::Point2F(mesh.tri.pt3.x, mesh.tri.pt3.y), D2D1::Point2F(mesh.tri.pt1.x, mesh.tri.pt1.y), _pDxBrush);
		}
	
		_pDxBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
		for (auto & waypoint : _waypoints)
		{
			GetRT()->FillEllipse(D2D1::Ellipse(D2D1::Point2F(waypoint.x, waypoint.y), 5, 5), _pDxBrush);
		}
	}
}

bool AStarWindow::InitBrush()
{
	if (nullptr == _pDxBrush)
	{
		GetRT()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_pDxBrush);
	}
	return nullptr != _pDxBrush;
}
