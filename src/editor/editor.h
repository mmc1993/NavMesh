#pragma once

#include "../window/window.h"
#include "../splitter/splitter.h"

class Editor : public Window {
public:
	static const float s_VERTEX_SIZE;

public:
	Editor();
	~Editor();

private:
	virtual void OnResize(int w, int h) override;
	virtual void OnKeyboard(int flag, int key) override;
	virtual void OnPaint(ID2D1HwndRenderTarget * pHRT) override;
	virtual void OnMouse(u_int msg, int x, int y, u_int key, int wheel) override;

	bool InitBrush(ID2D1HwndRenderTarget * pHRT);

	void OptInitVertex();
	void OptMeshWindow(int x, int y);
	void OptAppendVertex(int x, int y);
	void OptRemoveVertex(int x, int y);
	void OptWriteToFile(const stdstring & fname);

private:
	ID2D1SolidColorBrush * _pDxBrush;
	Splitter _splitter;
};