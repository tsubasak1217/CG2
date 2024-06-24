#include "Scene_Game.h"

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    gameState_ = std::make_unique<GameState_Test>("Test",this);
};

Scene_Game::~Scene_Game()
{
    //ChangeState(new GameState_Test("Test", this));
}

void Scene_Game::Initialize()
{
}

void Scene_Game::Finalize()
{
}

void Scene_Game::Update()
{
    gameState_->Update();
}

void Scene_Game::Draw()
{
    gameState_->Draw();
}

void Scene_Game::ChangeState(GameState_Base* newState)
{
    gameState_.reset(newState);
}
