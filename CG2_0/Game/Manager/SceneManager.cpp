#include "SceneManager.h"

SceneManager::SceneManager(SEED* pSEED)
{
    pScene_.reset(new Scene_Game(this,pSEED));
    pSEED_ = pSEED;
}

SceneManager::~SceneManager()
{
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
