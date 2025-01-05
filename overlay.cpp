#include "overlay.h"


HWND CreateOverlay(HWND targetWindow) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"OverlayClass";
    RegisterClassEx(&wc);

    RECT targetRect;
    GetWindowRect(targetWindow, &targetRect);

    HWND overlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        L"OverlayClass",
        L"Overlay",
        WS_POPUP | WS_VISIBLE,
        targetRect.left, targetRect.top,
        targetRect.right - targetRect.left,
        targetRect.bottom - targetRect.top,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    return overlay;
}