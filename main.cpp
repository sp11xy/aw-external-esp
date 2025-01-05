#include <iostream>
#include <Windows.h>
#include "structure.h"
#include "esp.h"


int main() {
    HWND hWindow = FindWindow(NULL, L"Call of Duty®: Advanced Warfare Multiplayer");
    if (hWindow == NULL) {
        std::cerr << "Could not find game window.\n";
        return 0;
    }

    DWORD procID;
    GetWindowThreadProcessId(hWindow, &procID);

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, procID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process!\n";
        return 0;
    }

    std::cout << "DEBUG BUILD v0.1\n";
    std::cout << "Process ID: " << procID << "\n";

    espDraw(hProcess, hWindow);

    CloseHandle(hProcess);
    return 0;
}