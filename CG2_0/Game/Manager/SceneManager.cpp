#include "SceneManager.h"

std::unique_ptr<SceneManager> SceneManager::pSceneManager_ = nullptr;
std::unique_ptr<Scene_Base> SceneManager::pScene_ = nullptr;

SceneManager::SceneManager()
{
    pScene_.reset(new Scene_Game(this));
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
    SceneManager sceneManager;
    pSceneManager_.reset(&sceneManager);
}

void SceneManager::Update()
{
    pScene_->Update();
}

void SceneManager::Draw()
{
    pScene_->Draw();
}

void SceneManager::ChangeScene(Scene_Base* newScene)
{
    pScene_.reset(newScene);
}
