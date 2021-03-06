// GetPos65535.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{
	POINT pos;
	HDC hdc = GetDC(NULL);
	int w = GetDeviceCaps(hdc, HORZRES);
	int h = GetDeviceCaps(hdc, VERTRES);
	
	while (true) {
		Sleep(100);
		GetCursorPos(&pos);
		printf("(%d,%d) in (%d,%d), (%.0lf, %.0lf), color = %x\n", pos.x, pos.y, w, h,
			0xffff*(float)pos.x/w,
			0xffff*(float)pos.y/h,
			GetPixel(hdc, pos.x, pos.y));
	}

	ReleaseDC(NULL, hdc);
    return 0;
}

