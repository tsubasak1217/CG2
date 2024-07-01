#include "SEED.h"
#include "Environment.h"
#include "SceneManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SEED::Initialize(hInstance, nCmdShow, "CG2", 640, 360);
    SceneManager::Initialize();

    while(SEED::ProcessMessage() != WM_QUIT){
        SEED::BeginFrame();

        SceneManager::Update();
        SceneManager::Draw();

        SEED::EndFrame();
    }

    SEED::Finalize();
    return 0;
}