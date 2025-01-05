#include <cmath>
#include <iostream>
#include <string>
#include "esp.h"
#include "overlay.h"

#define MAX_PLAYERS 18
uintptr_t DW_CG_t = 0x1414C66E0; //isSpectatingID, location
uintptr_t DW_Refdef = 0x1415575C0; //eyepos (w2s), axis(w2s), fov(w2s)
uintptr_t DW_ClientBase = 0x1414C1700; //teamID
uintptr_t DW_ClientSize = 0x330; //important for the loop
uintptr_t DW_EntityBase = 0x141655F60; //isAlive, eType, lerpOrigin
uintptr_t DW_EntitySize = 0x288; //important for the loop


void DrawBox(HDC hdc, int x, int y, int width, int height, COLORREF color) {
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x + width, y);
    LineTo(hdc, x + width, y + height);
    LineTo(hdc, x, y + height);
    LineTo(hdc, x, y);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}


void DrawTextAt(HDC hdc, int x, int y, const std::string& text, COLORREF color) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    TextOutA(hdc, x, y, text.c_str(), static_cast<int>(text.length()));
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

COLORREF GetThreatLevelColor(float distance) {
    // Distanz in Meter
    float distanceInMeters = distance / 100.0f;

    // Gefahrenlevel basierend auf Distanz
    if (distanceInMeters <= 10.0f) {
        return RGB(255, 0, 0); // Helle Gefahr (Rot)
    }
    else if (distanceInMeters <= 50.0f) {
        int greenValue = static_cast<int>((distanceInMeters - 10.0f) * 6.375f); // Von 0 bis 255
        return RGB(255, 255 - greenValue, 0); // Übergang von Rot nach Dunkelrot
    }
    else {
        return RGB(139, 0, 0); // Dunkelrot (geringe Gefahr)
    }
}


Vec3 w2s(const Vec3& point, const refdef_t& refdef) {
    Vec3 transform;
    Vec3 result;

    // Position relativ zur Kamera
    Vec3 position = point - refdef.eyePos;
    position.z += 25.0f;  // Höherer Offset für Box-Position


    // Dot products
    transform.x = position.x * refdef.axis_y.x + position.y * refdef.axis_y.y + position.z * refdef.axis_y.z;
    transform.y = position.x * refdef.axis_z.x + position.y * refdef.axis_z.y + position.z * refdef.axis_z.z;
    transform.z = position.x * refdef.axis_x.x + position.y * refdef.axis_x.y + position.z * refdef.axis_x.z;

    // Z-Check
    if (transform.z < 0.1f) {
        transform.z = 0;
        return transform;
    }

    // Screen center
    float centerX = 2560.0f * 0.5f;
    float centerY = 1440.0f * 0.5f;

    // Final calculation
    result.x = centerX * (1.0f - (transform.x / refdef.fov_x / transform.z));
    result.y = centerY * (1.0f - (transform.y / refdef.fov_y / transform.z));
    result.z = 1.0f;

    return result;
}


float calculateDistance(const Vec3& pos1, const Vec3& pos2) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float dz = pos1.z - pos2.z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}



void readGameData(HANDLE hProcess, refdef_t& refdef, centity_t entities[], int maxEntities) {
    // Lese Refdef (für späteren ESP wichtig)
    if (!ReadProcessMemory(hProcess, (LPCVOID)DW_Refdef, &refdef, sizeof(refdef_t), nullptr)) {
        std::cerr << "Failed to read Refdef structure!\n";
        return;
    }


    // Lese die Entitäten
    for (int i = 0; i < maxEntities; i++) {
        uintptr_t entityAddress = DW_EntityBase + (i * DW_EntitySize);
        if (!ReadProcessMemory(hProcess, (LPCVOID)entityAddress, &entities[i], sizeof(centity_t), nullptr)) {
            std::cerr << "Failed to read entity " << i << "!\n";
        }
    }
}

void DrawHealthBar(HDC hdc, int x, int y, int height, float healthPercent, COLORREF color) {
    int barWidth = 5;  // Breite der Healthbar
    int barHeight = static_cast<int>(height * healthPercent);  // Höhe basierend auf Prozent

    // Hintergrund der Healthbar (grau für nicht genutzte Health)
    HBRUSH bgBrush = CreateSolidBrush(RGB(50, 50, 50));
    RECT bgRect = { x, y, x + barWidth, y + height };
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);

    // Vordergrund der Healthbar (grün oder rot, je nach Health)
    HBRUSH fgBrush = CreateSolidBrush(color);
    RECT fgRect = { x, y + (height - barHeight), x + barWidth, y + height };
    FillRect(hdc, &fgRect, fgBrush);
    DeleteObject(fgBrush);
}

void readPlayerNames(HANDLE hProcess, playername_t playerNames[], int maxPlayers) {
    uintptr_t playerNamesBase = 0x141641CE9; // Basisadresse der Spielernamen
    for (int i = 0; i < maxPlayers; i++) {
        uintptr_t playerNameAddress = playerNamesBase + (i * 0x80); // Offset für jeden Spieler
        if (!ReadProcessMemory(hProcess, (LPCVOID)playerNameAddress, &playerNames[i], sizeof(playername_t), nullptr)) {
            std::cerr << "Failed to read player name for player " << i << "!\n";
        }
    }
}


void espDraw(HANDLE hProcess, HWND gameWindow) {
    refdef_t refdef;
    centity_t entities[MAX_PLAYERS];
    clientinfo_t clientInfos[MAX_PLAYERS];
    playername_t playerNames[MAX_PLAYERS];
    cg_t client;

    // Lese die Spielernamen
    readPlayerNames(hProcess, playerNames, MAX_PLAYERS);

    HWND overlayWindow = CreateOverlay(gameWindow);
    HDC hdc = GetDC(overlayWindow);

    while (true) {
        if (!IsWindow(gameWindow)) break;

        RECT targetRect;
        GetWindowRect(gameWindow, &targetRect);
        SetWindowPos(overlayWindow, HWND_TOPMOST,
            targetRect.left, targetRect.top,
            targetRect.right - targetRect.left,
            targetRect.bottom - targetRect.top,
            SWP_NOREDRAW);

        RECT rect;
        GetClientRect(overlayWindow, &rect);
        FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        readGameData(hProcess, refdef, entities, MAX_PLAYERS);

        if (!ReadProcessMemory(hProcess, (LPCVOID)DW_CG_t, &client, sizeof(cg_t), nullptr)) {
            std::cerr << "Failed to read player (CG) data!\n";
            continue;
        }

        Vec3 playerPos = client.location;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!ReadProcessMemory(hProcess, (LPCVOID)(DW_ClientBase + (i * DW_ClientSize)), &clientInfos[i], sizeof(clientinfo_t), nullptr)) {
                continue;
            }
        }

        int playerTeam = clientInfos[client.isSpectatingID].teamID;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            const centity_t& entity = entities[i];
            const clientinfo_t& clientInfo = clientInfos[i];

            if (!(entity.isAlive & 1) || (entity.eType != 1 && entity.eType != 3)) {
                continue;
            }

            if (i == client.isSpectatingID)
                continue;

            bool isEnemy = (clientInfo.teamID != playerTeam);
            Vec3 screenPos = w2s(entity.lerpOrigin, refdef);

            if (screenPos.z != 0) {
                float distance = calculateDistance(playerPos, entity.lerpOrigin);
                float boxHeight = 50.0f;
                float boxWidth = 25.0f;

                if (distance > 100.0f) {
                    boxHeight = (2000.0f / distance) * boxHeight;
                    boxWidth = (2000.0f / distance) * boxWidth;
                }

                boxHeight = max(30.0f, min(boxHeight, 300.0f));
                boxWidth = max(20.0f, min(boxWidth, 150.0f));

                COLORREF boxColor = isEnemy ? RGB(255, 0, 0) : RGB(0, 255, 0);
                int boxX = static_cast<int>(screenPos.x - boxWidth / 2);
                int boxY = static_cast<int>(screenPos.y - boxHeight / 2);
                int boxW = static_cast<int>(boxWidth);
                int boxH = static_cast<int>(boxHeight);

                // Zeichnen der Box
                DrawBox(hdc, boxX, boxY, boxW, boxH, boxColor);

                // Snapline-Endpunkt an die Unterkante der Box anpassen
                int snaplineEndX = boxX + boxW / 2; // Horizontale Mitte der Box
                int snaplineEndY = boxY + boxH;     // Unterkante der Box

                // Dynamische Snapline basierend auf Gefahrenlevel zeichnen
                int screenCenterX = 2560 / 2; // Bildschirmmitte
                int screenBottomY = 1440;    // Unterkante des Bildschirms
                COLORREF lineColor = isEnemy ? GetThreatLevelColor(distance) : RGB(0, 255, 0);
                DrawLine(hdc, screenCenterX, screenBottomY, snaplineEndX, snaplineEndY, lineColor);

                // Spielernamen anzeigen
                std::string playerName = std::string(playerNames[i].name);
                DrawTextAt(hdc, boxX, boxY - 25, playerName, RGB(255, 255, 255)); // Name über der Box

                // Distanz anzeigen
                std::string distanceText = std::to_string(static_cast<int>(distance / 100)) + "m";
                DrawTextAt(hdc, boxX, boxY - 40, distanceText, RGB(255, 255, 255)); // Distanz unter dem Namen
            }
        }
        Sleep(1);
    }

    ReleaseDC(overlayWindow, hdc);
    DestroyWindow(overlayWindow);
}
