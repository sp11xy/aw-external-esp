#pragma once
#include <Windows.h>
#include "structure.h"

Vec3 w2s(const Vec3& point, const refdef_t& refdef);
float calculateDistance(const Vec3& pos1, const Vec3& pos2);
void DrawBox(HDC hdc, int x, int y, int width, int height, COLORREF color);
void espDraw(HANDLE hProcess, HWND gameWindow);

