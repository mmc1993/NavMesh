#include "editor.h"

const float Editor::s_VERTEX_SIZE = 5.0f;

Editor::Editor() : _pDxBrush(nullptr)
{
}

Editor::~Editor()
{
	SAFE_RELEASE(_pDxBrush);
}

void Editor::OnResize(int w, int h)
{
}

void Editor::OnKeyboard(int flag, int key)
{
}

void Editor::OnPaint(ID2D1HwndRenderTarget * pHRT)
{
	if (InitBrush(pHRT))
	{
		for (const auto & vertex : _splitter.GetVertexs())
		{
			pHRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vertex.x, vertex.y), s_VERTEX_SIZE, s_VERTEX_SIZE), _pDxBrush);
		}
		for (const auto & triangle : _splitter.GetTriangles())
		{
			pHRT->DrawLine(D2D1::Point2F(triangle.pt1.x, triangle.pt1.y),
				D2D1::Point2F(triangle.pt2.x, triangle.pt2.y), _pDxBrush);
			pHRT->DrawLine(D2D1::Point2F(triangle.pt2.x, triangle.pt2.y),
				D2D1::Point2F(triangle.pt3.x, triangle.pt3.y), _pDxBrush);
			pHRT->DrawLine(D2D1::Point2F(triangle.pt3.x, triangle.pt3.y),
				D2D1::Point2F(triangle.pt1.x, triangle.pt1.y), _pDxBrush);
		}
	}
}

void Editor::OnMouse(u_int msg, int x, int y, u_int key, int wheel)
{
	static int s_x;
	static int s_y;
	switch (msg)
	{
	case WM_LBUTTONUP:
		OptAppendVertex(x, y);
		break;
	case WM_RBUTTONUP:
		OptRemoveVertex(x, y);
		break;
	case WM_MBUTTONUP:
		OptInitVertex();
		break;
	}
}

bool Editor::InitBrush(ID2D1HwndRenderTarget * pHRT)
{
	if (nullptr == _pDxBrush)
	{
		pHRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_pDxBrush);

		OptInitVertex();
	}
	return nullptr != _pDxBrush;
}

void Editor::OptInitVertex()
{
	auto & size = GetContent();
	_splitter.Clear();
	_splitter.SetHelperVertex({math::Vec2(0.0f, 0.0f), math::Vec2((float)size.cx, 0.0f),
		math::Vec2((float)size.cx, (float)size.cy), math::Vec2(0.0f, (float)size.cy) });
}

void Editor::OptMeshWindow(int x, int y)
{
}

void Editor::OptAppendVertex(int x, int y)
{
	_splitter.AppendVertex(math::Vec2((float)x, (float)y));
}

void Editor::OptRemoveVertex(int x, int y)
{
	auto downPt = math::Vec2((float)x, (float)y);
	auto iter = std::find_if(_splitter.GetVertexs().begin(), _splitter.GetVertexs().end(), [&downPt](const math::Vec2 & pt)
		{
			return (downPt - pt).Length() <= s_VERTEX_SIZE;
		});
	if (iter != _splitter.GetVertexs().end())
	{
		_splitter.RemoveVertex(*iter);
	}
}

void Editor::OptWriteToFile(const stdstring & fname)
{
}
