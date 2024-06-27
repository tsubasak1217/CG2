#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;
Scene_Base* SceneManager::pScene_ = nullptr;

SceneManager::SceneManager()
{
    pScene_ = new Scene_Game(instance_);
}

SceneManager::~SceneManager()
{
    delete pScene_;
    pScene_ = nullptr;
    delete instance_;
    instance_ = nullptr;
}

void SceneManager::Initialize()
{
    instance_ = GetInstance();
}

void SceneManager::Update()
{
    pScene_->Update();
}

void SceneManager::Draw()
{
    pScene_->Draw();
}

SceneManager* SceneManager::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(
        onceFlag,
        [](){
        if(!instance_){ instance_ = new SceneManager; }
    });

    return instance_;
}

void SceneManager::ChangeScene(Scene_Base* newScene)
{
    delete pScene_;
    pScene_ = nullptr;

    pScene_ = newScene;
}
