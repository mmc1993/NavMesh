#include "window/window.h"
#include "editor/editor.h"
#include "../resource.h"

BOOL CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
{
	return FALSE;
}

int main()
{
	Editor editor;
	editor.Create(TEXT("Íø¸ñµ¼º½"), 100, 100, 800, 400);
	editor.Loop();
	return 0;
}

