// Pixer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{
	HWND hConsole = FindWindow(NULL, L"阴阳师-网易游戏");
	HDC hdc = GetDC(hConsole);
	MSG msg = { 0 };
	RegisterHotKey(hConsole, 1, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'Z');
	// 循环获取操作系统发来的消息
	POINT pos;
	printf("DWORD sig[][3]={");
	for (int i = 0; i < 5; i++) {
		getchar();
		GetCursorPos(&pos);
		printf("{%d, %d, 0x%x},", pos.x, pos.y, GetPixel(hdc, pos.x, pos.y)&0x00ffffff);
	}
	printf("};");
	ReleaseDC(hConsole, hdc);

    return 0;
}

