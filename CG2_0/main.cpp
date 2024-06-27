#include "SEED.h"
#include "Environment.h"
#include "SceneManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SEED::Initialize(hInstance, nCmdShow, "CG2", 1280, 720);
    SceneManager::Initialize();

    while(SEED::GetInstance()->msg_.message != WM_QUIT){
        SEED::BeginFrame();

        SceneManager::Update();
        SceneManager::Draw();

        SEED::EndFrame();
    }

    SEED::Finalize();
    return 0;
}