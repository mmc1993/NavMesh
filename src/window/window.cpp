#include "window.h"

#pragma comment (lib, "d2d1.lib")

Window::Window()
	: _hWnd(0)
	, _pDxHRT(nullptr)
	, _pDxFactory(nullptr)
{ }

Window::~Window()
{
	SAFE_RELEASE(_pDxHRT);
	SAFE_RELEASE(_pDxFactory);
}

void Window::Create(const stdstring & title, int x, int y, int w, int h)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &WindowMessage;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszClassName = TEXT("mmc");
	if (RegisterClassEx(&wc))
	{
		//	创建窗口
		CreateWindow(TEXT("mmc"), title.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
			CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, this);

		//	初始化DX
		auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &_pDxFactory);
		if (SUCCEEDED(hr))
		{
			hr = _pDxFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(
					GetHandle(), D2D1::SizeU(w, h)), &_pDxHRT);
		}
		if (SUCCEEDED(hr))
		{
			Move(x, y, w, h); 
			Show(true);
		}
	}
}

void Window::Move(int x, int y, int w, int h)
{
	RECT rc1, rc2;
	GetWindowRect(GetHandle(), &rc1);
	GetClientRect(GetHandle(), &rc2);
	auto diffw = rc1.right - rc1.left - rc2.right;
	auto diffh = rc1.bottom - rc1.top - rc2.bottom;
	MoveWindow(GetHandle(), x, y, w + diffw, h + diffh, TRUE);
}

void Window::Show(bool visible)
{
	ShowWindow(GetHandle(), visible);
}

void Window::Loop()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UpdateWindow(GetHandle());
	}
}

RECT Window::GetRect()
{
	auto xy = GetPosition();
	auto wh = GetContent();
	return { xy.x, xy.y, wh.cx, wh.cy };
}

HWND Window::GetHandle()
{
	return _hWnd;
}

SIZE Window::GetContent()
{
	RECT rc;
	GetClientRect(GetHandle(), &rc);
	return { rc.right, rc.bottom };
}

POINT Window::GetPosition()
{
	RECT rc;
	GetWindowRect(GetHandle(), &rc);
	return { rc.left, rc.top };
}

LRESULT Window::WindowMessage(HWND hWnd, UINT uInt, WPARAM wParam, LPARAM lParam)
{
	if (WM_CREATE == uInt)
	{
		auto param = reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams;
		auto pWindow = reinterpret_cast<Window *>(param);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)param);
		pWindow->_hWnd = hWnd;
		return TRUE;
	}
	auto pWindow = reinterpret_cast<Window *>(GetWindowLong(hWnd, GWL_USERDATA));
	switch (uInt)
	{
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, pWindow->Cls_OnLButtonDown);
		HANDLE_MSG(hWnd, WM_MBUTTONDOWN, pWindow->Cls_OnMButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, pWindow->Cls_OnRButtonDown);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, pWindow->Cls_OnMouseWheel);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, pWindow->Cls_OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, pWindow->Cls_OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MBUTTONUP, pWindow->Cls_OnMButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, pWindow->Cls_OnRButtonUp);
		HANDLE_MSG(hWnd, WM_CLOSE, pWindow->Cls_OnClose);
		HANDLE_MSG(hWnd, WM_PAINT, pWindow->Cls_OnPaint);
		HANDLE_MSG(hWnd, WM_KEYDOWN, pWindow->Cls_OnKey);
		HANDLE_MSG(hWnd, WM_KEYUP, pWindow->Cls_OnKey);
		HANDLE_MSG(hWnd, WM_SIZE, pWindow->Cls_OnSize);
	}
	return DefWindowProc(hWnd, uInt, wParam, lParam);
}

void Window::Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	OnMouse(WM_LBUTTONDOWN, x, y, keyFlags, 0);
}

void Window::Cls_OnMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	OnMouse(WM_MBUTTONDOWN, x, y, keyFlags, 0);
}

void Window::Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	OnMouse(WM_RBUTTONDOWN, x, y, keyFlags, 0);
}

void Window::Cls_OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	OnMouse(WM_MOUSEWHEEL, xPos, yPos, fwKeys, zDelta);
}

void Window::Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT keyFlags)
{
	OnKeyboard(keyFlags, vk);
}

void Window::Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	OnMouse(WM_LBUTTONUP, x, y, keyFlags, 0);
}

void Window::Cls_OnMButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	OnMouse(WM_MBUTTONUP, x, y, keyFlags, 0);
}

void Window::Cls_OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	OnMouse(WM_RBUTTONUP, x, y, keyFlags, 0);
}

void Window::Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	OnMouse(WM_MOUSEMOVE, x, y, keyFlags, 0);
}

void Window::Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	_pDxHRT->Resize(D2D1::SizeU(cx, cy));
	OnResize(cx, cy); 
}

void Window::Cls_OnPaint(HWND hwnd)
{
	_pDxHRT->BeginDraw();
	_pDxHRT->Clear();
	OnPaint(_pDxHRT);
	_pDxHRT->EndDraw();
}

void Window::Cls_OnClose(HWND hwnd)
{
	PostQuitMessage(0);
}
