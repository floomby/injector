#include <windows.h>

int blah()
{
    MessageBoxA(NULL, "attached", "attached", MB_OK);
    return GetLastError();
}

extern "C" BOOL APIENTRY __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason){
        case DLL_PROCESS_ATTACH:
            Sleep(3000);
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

