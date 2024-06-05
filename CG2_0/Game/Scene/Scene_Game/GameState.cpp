#include "GameState.h"
#include "SEED.h"
#include "Scene_Game.h"
#include "includes.h"
#include "ShapeMath.h"
#include "Environment.h"
#include <ImGui.h>
#include <random>

/*==================================================================================================*/
/*                                             確認シーン                                            */
/*==================================================================================================*/

GameState_Test::GameState_Test(const std::string& name, Scene_Game* pScene) : GameState_Base(name, pScene){

}

GameState_Test::~GameState_Test()
{
}

void GameState_Test::Update()
{
    //ImGui::Begin("");
    //ImGui::End();
}

void GameState_Test::Draw()
{
}

/*==================================================================================================*/
/*                                             演出シーン                                            */
/*==================================================================================================*/

GameState_Effect::GameState_Effect(const std::string& name, Scene_Game* pScene) : GameState_Base(name, pScene){}

GameState_Effect::~GameState_Effect()
{
}

void GameState_Effect::Update()
{
}

void GameState_Effect::Draw()
{
}
