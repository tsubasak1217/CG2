#include <windows.h>
#include <cstdint>
#include "myFunc.h"
#include "matrixFunc.h"
#include "sphere.h"
#include "includes.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "SEED.h"

int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE,
    LPSTR, int nCmdShow
) {
    SEED::Run(hInstance,nCmdShow,"CG2_Triangle", 1280, 720);
    return 0;
}