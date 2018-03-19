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
	if (WM_KEYUP == msg)
	{
		//	初始网格, 烘培网格 & 保存网格
		switch (key) 
		{
		case 'I': OptInitMesh(); break;
		case 'B': OptBakeMesh(); break;
		}
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

		OptMeshClear();
	}
	return nullptr != _pDxBrush;
}

bool MeshWindow::OptInitMesh()
{
	_meshs.clear();
	std::copy(_mesher.GetTriangles().begin(),
			  _mesher.GetTriangles().end(),
			  std::back_inserter(_meshs));
	Log("OptInitMesh...");
	return true;
}

bool MeshWindow::OptBakeMesh()
{
	std::vector<Mesh> meshs;
	std::copy_if(_meshs.begin(), _meshs.end(), std::back_inserter(meshs), [](const Mesh & mesh)
				 {
					 return mesh.attr == MeshAttr::kOPEN;
				 });
	std::for_each(meshs.begin(), meshs.end(), [this, &meshs](Mesh & mesh) 
				  { 
					  LinkMesh(meshs, mesh);
					  SortVertex(mesh);
				  });
	SaveMesh(meshs, "nav.txt");
	Log("OptBakeMesh...");
	return true;
}

void MeshWindow::LinkMesh(const std::vector<Mesh> & meshs, Mesh & mesh)
{
	auto insert = mesh.links.begin();
	for (const auto & iter : meshs)
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

void MeshWindow::SaveMesh(const std::vector<Mesh>& meshs, const std::string & fname)
{
	std::string buffer;
	for (const auto & mesh : meshs)
	{
		auto slink = std::string();
		auto count = std::count(mesh.links.begin(), mesh.links.end(), nullptr);
		for (auto i = 0; i != mesh.links.size() - count; ++i)
		{
			slink.append(std::to_string(std::distance(
				static_cast<const Mesh *>(meshs.data()), mesh.links.at(i))));
			slink.append(" ");
		}
		auto cp = mesh.tri.GetCenterPoint();
		buffer.append(SFormat("< cp {0} {1} , pt1 {2} {3} , pt2 {4} {5} , pt3 {6} {7} , link {8} {9}>\n",
							  cp.x, cp.y,
							  mesh.tri.pt1.x, mesh.tri.pt1.y,
							  mesh.tri.pt2.x, mesh.tri.pt2.y,
							  mesh.tri.pt3.x, mesh.tri.pt3.y,
							  mesh.links.size() - count, slink));
	}
	std::ofstream ofile(fname);
	ofile << std::noskipws << buffer;
	ofile.close();
}

bool MeshWindow::OptMeshClear()
{
	auto & size = GetContent();
	_mesher.Clear();
	_mesher.SetHelperVertex({math::Vec2(0.0f, 0.0f), math::Vec2((float)size.cx, 0.0f),
		math::Vec2((float)size.cx, (float)size.cy), math::Vec2(0.0f, (float)size.cy)});
	OptInitMesh();
	Log("OptMeshClear...");
	return true;
}

bool MeshWindow::OptMeshModify(int x, int y)
{
	auto iter = std::find_if(_meshs.begin(), _meshs.end(), [x, y](const auto & mesh)
		{
			return math::IsPointInTriangle(mesh.tri.pt1, mesh.tri.pt2, mesh.tri.pt3, { (float)x, (float)y });
		});
	DialogMeshAttr::Attr newAttr = Dialog::Open<DialogMeshAttr::Attr>(DialogMeshAttr(GetHandle()),
		DialogMeshAttr::Attr(std::distance(_meshs.begin(), iter), (std::uint8_t)iter->attr));
	iter->attr = (MeshAttr)newAttr.attr;
	Log("OptMeshModify...");
	return true;
}

bool MeshWindow::OptAppendVertex(int x, int y)
{
	auto result = false;
	auto downPt = math::Vec2((float)x, (float)y);
	auto iter = std::find_if(_mesher.GetVertexs().begin(), 
							 _mesher.GetVertexs().end(), 
							 [&downPt](const auto & pt)
							 {
								 return (downPt - pt).Length() <= s_VERTEX_SPAC;
							 });
	if (iter == _mesher.GetVertexs().end())
	{
		_mesher.AppendVertex(math::Vec2((float)x, (float)y));
		result = true;
	}
	Log("OptAppendVertex...");
	return result;
}

bool MeshWindow::OptRemoveVertex(int x, int y)
{
	auto result = false;
	auto downPt = math::Vec2((float)x, (float)y);
	auto iter = std::find_if(_mesher.GetVertexs().begin(), 
							 _mesher.GetVertexs().end(), 
							 [&downPt](const auto & pt)
							 {
								 return (downPt - pt).Length() <= s_VERTEX_SIZE;
							 });
	if (iter != _mesher.GetVertexs().end())
	{
		_mesher.RemoveVertex(*iter);
		result = true;
	}
	Log("OptRemoveVertex...");
	return result;
}

void MeshWindow::OnLButtonUP(int x, int y, u_int key, int wheel)
{
	OptAppendVertex(x, y);
}

void MeshWindow::OnRButtonUP(int x, int y, u_int key, int wheel)
{
	if (!OptRemoveVertex(x, y))
	{
		OptMeshModify(x, y);
	}
}

void MeshWindow::OnMButtonUP(int x, int y, u_int key, int wheel)
{
	OptMeshClear();
}
