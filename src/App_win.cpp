
#include <winsock2.h>
#include <windows.h>


#include "App.h"
#include "core/Controller.h"


bool xmrig::App::background(int &)
{
    if (!m_controller->isBackground()) {
        return false;
    }

    HWND hcon = GetConsoleWindow();
    if (hcon) {
        ShowWindow(hcon, SW_HIDE);
    } else {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        CloseHandle(h);
        FreeConsole();
    }

    return false;
}
