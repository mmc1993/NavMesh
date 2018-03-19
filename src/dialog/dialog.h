#pragma once

#include "../base.h"
#include <Windows.h>
#include <windowsx.h>
#include "../../resource.h"

class Dialog {
public:
	template <class Output, class Dialog, class ...Inputs>
	static Output Open(Dialog & dialog, Inputs && ...inputs)
	{
		return dialog.Open(std::forward<Inputs...>(inputs...));
	}

public:
	Dialog(HWND parent): _hParent(parent)
	{ }

	virtual ~Dialog()
	{ }

	HWND GetParentWindow()
	{
		return _hParent;
	}

private:
	HWND _hParent;

};

//	Õ¯∏Ò Ù–‘
class DialogMeshAttr : public Dialog {
public:
	struct Attr {
		Attr()
		{ }
		Attr(std::uint8_t i, std::uint8_t a): idx(i), attr(a)
		{ }
		std::uint8_t idx;
		std::uint8_t attr;
	};

	DialogMeshAttr(HWND parent): Dialog(parent)
	{ }

	Attr Open(const Attr & attr)
	{
		auto ret = attr;
		DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_MESHATTR), GetParentWindow(), WndProc, (LPARAM)&ret);
		return ret;
	}

private:
	static BOOL CALLBACK WndProc(HWND hwnd, UINT uInt, WPARAM wParam, LPARAM lParam)
	{
		switch (uInt)
		{
		case WM_INITDIALOG:
			{
				const auto & attr = *(Attr *)(lParam);
				SetDlgItemInt(hwnd, IDC_INDEX, attr.idx, TRUE);
				SetDlgItemInt(hwnd, IDC_VALUE, (UINT)attr.attr, TRUE);
				SetWindowLong(hwnd, GWL_USERDATA, lParam);

				POINT point;
				GetCursorPos(&point);
				SetWindowPos(hwnd, 0, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			break;
		case WM_COMMAND:
			{
				if (IDB_OK == wParam)
				{
					auto & attr = *(Attr *)GetWindowLong(hwnd, GWL_USERDATA);
					attr.attr = GetDlgItemInt(hwnd, IDC_VALUE, nullptr, FALSE);
				}
				if (IDB_NO == wParam || IDB_OK == wParam)
				{
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				}
			}
			break;
		case WM_CLOSE:
			{
				EndDialog(hwnd, 0);
			}
			break;
		}
		return FALSE;
	}

};