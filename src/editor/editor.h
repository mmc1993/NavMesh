#pragma once

#include "../window/window.h"
#include "../mesher/mesher.h"

class Editor : public Window {
public:
	static const float s_VERTEX_SIZE;
	static const float s_VERTEX_SPAC;

	enum class MeshAttr {
		kOPEN,
		kCLOSE,
	};

	struct Mesh {
		Mesh()
		{ }
		Mesh(const Mesher::Triangle & t): tri(t), attr(MeshAttr::kOPEN)
		{ }
		Mesher::Triangle tri;
		MeshAttr attr;
	};

public:
	Editor();
	~Editor();

private:
	virtual void OnMouse(u_int msg, int x, int y, u_int key, int wheel) override;
	virtual void OnKey(u_int msg, int flag, int key) override;
	virtual void OnResize(int w, int h) override;
	virtual void OnPaint() override;

	bool InitBrush();
	bool OptInitVertex();
	bool OptRenderMesh();
	bool OptMeshWindow(int x, int y);
	bool OptAppendVertex(int x, int y);
	bool OptRemoveVertex(int x, int y);
	bool OptWriteToFile(const stdstring & fname);
	void OnLButtonUP(int x, int y, u_int key, int wheel);
	void OnRButtonUP(int x, int y, u_int key, int wheel);
	void OnMButtonUP(int x, int y, u_int key, int wheel);

private:
	Mesher _mesher;
	std::vector<Mesh> _meshs;
	ID2D1SolidColorBrush * _pDxBrush;
};