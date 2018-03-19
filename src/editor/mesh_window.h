#pragma once

#include "../window/window.h"
#include "../mesher/mesher.h"

class MeshWindow : public Window {
public:
	static const float s_VERTEX_SIZE;
	static const float s_VERTEX_SPAC;

	enum class MeshAttr {
		kOPEN,
		kCLOSE,
	};

	struct Mesh {
		Mesh()
		{ 
			links.fill(nullptr);
		}
		Mesh(const math::Triangle & t): tri(t), attr(MeshAttr::kOPEN)
		{
			links.fill(nullptr);
		}
		operator const math::Triangle &() const
		{
			return tri;
		}
		MeshAttr attr;
		math::Triangle tri;
		std::array<const Mesh *, 3> links;
	};

public:
	MeshWindow();
	~MeshWindow();

private:
	virtual void OnMouse(u_int msg, int x, int y, u_int key, int wheel) override;
	virtual void OnKey(u_int msg, int flag, int key) override;
	virtual void OnResize(int w, int h) override;
	virtual void OnPaint() override;

	bool InitBrush();
	bool OptInitMesh();
	bool OptBakeMesh();
	void SortVertex(Mesh & mesh);
	void LinkMesh(const std::vector<Mesh> & meshs, Mesh & mesh);
	void SaveMesh(const std::vector<Mesh> & meshs, const std::string & fname);
	bool OptMeshClear();
	bool OptMeshModify(int x, int y);
	bool OptAppendVertex(int x, int y);
	bool OptRemoveVertex(int x, int y);
	void OnLButtonUP(int x, int y, u_int key, int wheel);
	void OnRButtonUP(int x, int y, u_int key, int wheel);
	void OnMButtonUP(int x, int y, u_int key, int wheel);

private:
	Mesher _mesher;
	std::vector<Mesh> _meshs;
	ID2D1SolidColorBrush * _pDxBrush;
};