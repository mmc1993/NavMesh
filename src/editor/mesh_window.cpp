#include "mesh_window.h"
#include "../dialog/dialog.h"

const float MeshWindow::s_VERTEX_SIZE = 5.0f;
const float MeshWindow::s_VERTEX_SPAC = 100.0f;

MeshWindow::MeshWindow() : _pDxBrush(nullptr)
{
}

MeshWindow::~MeshWindow()
{
	SafeRelease(_pDxBrush);
}

void MeshWindow::OnResize(int w, int h)
{
}

void MeshWindow::OnKey(u_int msg, int flag, int key)
{
	if (WM_KEYUP == msg && 'B' == key)
	{
		OptBuildMesh();
	}
	else if (WM_KEYUP == msg && 'S' == key)
	{
		OptWriteToFile("nav.txt");
	}
}

void MeshWindow::OnPaint()
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

void MeshWindow::OnMouse(u_int msg, int x, int y, u_int key, int wheel)
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

bool MeshWindow::InitBrush()
{
	if (nullptr == _pDxBrush)
	{
		GetRT()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_pDxBrush);

		OptResetVertex();
	}
	return nullptr != _pDxBrush;
}

bool MeshWindow::OptBuildMesh()
{
	Log("OptBuildMesh Begin");
	_meshs.clear();
	std::copy(_mesher.GetTriangles().begin(),
			  _mesher.GetTriangles().end(),
			  std::back_inserter(_meshs));
	for (auto & mesh : _meshs)
	{
		NearMesh(mesh);
		SortVertex(mesh);
	}
	Log("OptBuildMesh End");
	return true;
}

void MeshWindow::NearMesh(Mesh & mesh)
{
	auto insert = mesh.nears.begin();
	for (const auto & iter : _meshs)
	{
		if (mesh.tri.QueryCommonLine(iter.tri))
		{
			*insert++ = &iter;
		}
	}
}

void MeshWindow::SortVertex(Mesh & mesh)
{
	if ((mesh.tri.pt2 - mesh.tri.pt1).Cross(mesh.tri.pt3 - mesh.tri.pt2) < 0)
	{
		std::swap(mesh.tri.pt2, mesh.tri.pt3);
	}
}

bool MeshWindow::OptResetVertex()
{
	Log("OpResetVertex Begin");
	auto & size = GetContent();
	_mesher.Clear();
	_mesher.SetHelperVertex({math::Vec2(0.0f, 0.0f), math::Vec2((float)size.cx, 0.0f),
		math::Vec2((float)size.cx, (float)size.cy), math::Vec2(0.0f, (float)size.cy)});
	OptBuildMesh();
	Log("OpResetVertex End");
	return true;
}

bool MeshWindow::OptMeshWindow(int x, int y)
{
	Log("OptMeshWindow Begin");
	auto iter = std::find_if(_meshs.begin(), _meshs.end(), [x, y](const auto & mesh)
		{
			return math::IsPointInTriangle(mesh.tri.pt1, mesh.tri.pt2, mesh.tri.pt3, { (float)x, (float)y });
		});
	DialogMeshAttr::Attr newAttr = Dialog::Open<DialogMeshAttr::Attr>(DialogMeshAttr(GetHandle()),
		DialogMeshAttr::Attr(std::distance(_meshs.begin(), iter), (std::uint8_t)iter->attr));
	iter->attr = (MeshAttr)newAttr.attr;
	Log("OptMeshWindow End");
	return true;
}

bool MeshWindow::OptAppendVertex(int x, int y)
{
	Log("OptAppendVertex Begin");
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
	Log("OptAppendVertex End");
	return true;
}

bool MeshWindow::OptRemoveVertex(int x, int y)
{
	Log("OptRemoveVertex Begin");
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
	Log("OptRemoveVertex End");
	return false;
}

bool MeshWindow::OptWriteToFile(const std::string & fname)
{
	Log("OptWriteToFile Begin");
	std::string buffer;
	for (const auto & mesh : _meshs)
	{
		auto snear = std::string("near ");
		auto count = std::count(mesh.nears.begin(), mesh.nears.end(), nullptr);
		for (auto i = 0; i != mesh.nears.size() - count; ++i)
		{
			snear.append(std::to_string(std::distance(
				static_cast<const Mesh *>(_meshs.data()), mesh.nears.at(i))));
			snear.append(" ");
		}
		auto cp = mesh.tri.GetCenterPoint();
		buffer.append(SFormat("< cp {0} {1} , pt1 {2} {3} , pt2 {4} {5} , pt3 {6} {7} , open {8} , {9}>\n",
							  cp.x, cp.y, 
							  mesh.tri.pt1.x, mesh.tri.pt1.y,
							  mesh.tri.pt2.x, mesh.tri.pt2.y,
							  mesh.tri.pt3.x, mesh.tri.pt3.y,
							  (std::uint8_t)mesh.attr, snear));
	}
	std::ofstream ofile(fname);
	ofile << std::noskipws << buffer;
	ofile.close();
	Log("OptWriteToFile End");
	return true;
}

void MeshWindow::OnLButtonUP(int x, int y, u_int key, int wheel)
{
	OptAppendVertex(x, y);
}

void MeshWindow::OnRButtonUP(int x, int y, u_int key, int wheel)
{
	if (!OptRemoveVertex(x, y))
	{
		OptMeshWindow(x, y);
	}
}

void MeshWindow::OnMButtonUP(int x, int y, u_int key, int wheel)
{
	OptResetVertex();
}
