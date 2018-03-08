#include "editor.h"
#include "../dialog/dialog.h"

const float Editor::s_VERTEX_SIZE = 5.0f;
const float Editor::s_VERTEX_SPAC = 100.0f;

Editor::Editor() : _pDxBrush(nullptr)
{
}

Editor::~Editor()
{
	SafeRelease(_pDxBrush);
}

void Editor::OnResize(int w, int h)
{
}

void Editor::OnKey(u_int msg, int flag, int key)
{
	if (WM_KEYUP == msg && VK_RETURN == key)
	{
		OptRenderMesh();
		std::cout << "render mesh ok" << std::endl;
	}
}

void Editor::OnPaint()
{
	if (InitBrush())
	{
		_pDxBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		for (const auto & triangle : _mesher.GetTriangles())
		{
			GetRT()->DrawLine(D2D1::Point2F(triangle.pt1.x, triangle.pt1.y), D2D1::Point2F(triangle.pt2.x, triangle.pt2.y), _pDxBrush);
			GetRT()->DrawLine(D2D1::Point2F(triangle.pt2.x, triangle.pt2.y), D2D1::Point2F(triangle.pt3.x, triangle.pt3.y), _pDxBrush);
			GetRT()->DrawLine(D2D1::Point2F(triangle.pt3.x, triangle.pt3.y), D2D1::Point2F(triangle.pt1.x, triangle.pt1.y), _pDxBrush);
		}

		_pDxBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
		for (const auto & mesh : _meshs) 
		{
			if (mesh.attr == MeshAttr::kCLOSE)
			{
				GetRT()->DrawLine({ mesh.tri.pt1.x, mesh.tri.pt1.y }, { mesh.tri.pt2.x, mesh.tri.pt2.y }, _pDxBrush);
				GetRT()->DrawLine({ mesh.tri.pt2.x, mesh.tri.pt2.y }, { mesh.tri.pt3.x, mesh.tri.pt3.y }, _pDxBrush);
				GetRT()->DrawLine({ mesh.tri.pt3.x, mesh.tri.pt3.y }, { mesh.tri.pt1.x, mesh.tri.pt1.y }, _pDxBrush);
			}
		}

		_pDxBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		for (const auto & vertex : _mesher.GetVertexs())
		{
			GetRT()->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vertex.x, vertex.y), s_VERTEX_SIZE, s_VERTEX_SIZE), _pDxBrush);
		}
	}
}

void Editor::OnMouse(u_int msg, int x, int y, u_int key, int wheel)
{
	switch (msg)
	{
	case WM_LBUTTONUP:
		OnLButtonUP(x, y, key, wheel);
		break;
	case WM_RBUTTONUP:
		OnRButtonUP(x, y, key, wheel);
		break;
	case WM_MBUTTONUP:
		OnMButtonUP(x, y, key, wheel);
		break;
	}
}

bool Editor::InitBrush()
{
	if (nullptr == _pDxBrush)
	{
		GetRT()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_pDxBrush);

		OptInitVertex();
	}
	return nullptr != _pDxBrush;
}

bool Editor::OptInitVertex()
{
	auto & size = GetContent();
	_mesher.Clear();
	_mesher.SetHelperVertex({math::Vec2(0.0f, 0.0f), math::Vec2((float)size.cx, 0.0f),
		math::Vec2((float)size.cx, (float)size.cy), math::Vec2(0.0f, (float)size.cy)});
	OptRenderMesh();
	return true;
}

bool Editor::OptRenderMesh()
{
	_meshs.clear();
	std::copy(
		_mesher.GetTriangles().begin(),
		_mesher.GetTriangles().end(), 
		std::back_inserter(_meshs));
	return true;
}

bool Editor::OptMeshWindow(int x, int y)
{
	auto iter = std::find_if(_meshs.begin(), _meshs.end(), [x, y](const auto & mesh)
		{
			return math::IsPointInTriangle(mesh.tri.pt1, mesh.tri.pt2, mesh.tri.pt3, { (float)x, (float)y });
		});
	DialogMeshAttr::Attr newAttr = Dialog::Open<DialogMeshAttr::Attr>(DialogMeshAttr(GetHandle()),
		DialogMeshAttr::Attr(std::distance(_meshs.begin(), iter), (std::uint8_t)iter->attr));
	iter->attr = (MeshAttr)newAttr.attr;
	return true;
}

bool Editor::OptAppendVertex(int x, int y)
{
	auto downPt = math::Vec2((float)x, (float)y);
	auto iter = std::find_if(_mesher.GetVertexs().begin(), _mesher.GetVertexs().end(), [&downPt](const auto & pt)
		{
			return (downPt - pt).Length() <= s_VERTEX_SPAC;
		});
	if (iter == _mesher.GetVertexs().end())
	{
		_mesher.AppendVertex(math::Vec2((float)x, (float)y));
		return true;
	}
	return true;
}

bool Editor::OptRemoveVertex(int x, int y)
{
	auto downPt = math::Vec2((float)x, (float)y);
	auto iter = std::find_if(_mesher.GetVertexs().begin(), _mesher.GetVertexs().end(), [&downPt](const auto & pt)
		{
			return (downPt - pt).Length() <= s_VERTEX_SIZE;
		});
	if (iter != _mesher.GetVertexs().end())
	{
		_mesher.RemoveVertex(*iter);
		return true;
	}
	return false;
}

bool Editor::OptWriteToFile(const stdstring & fname)
{
	return true;
}

void Editor::OnLButtonUP(int x, int y, u_int key, int wheel)
{
	OptAppendVertex(x, y);
}

void Editor::OnRButtonUP(int x, int y, u_int key, int wheel)
{
	if (!OptRemoveVertex(x, y))
	{
		OptMeshWindow(x, y);
	}
}

void Editor::OnMButtonUP(int x, int y, u_int key, int wheel)
{
	OptInitVertex();
}
