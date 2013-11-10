#include <Global.h>
#include <Application.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!sApplication->Init())
        return -1;

    return sApplication->Run();
}
