#include "SEED.h"
#include "Environment.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SEED::Run(hInstance,nCmdShow,kWindowTitle, kWindowSizeX, kWindowSizeY);
    return 0;
}