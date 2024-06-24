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
    for(int i = 0; i < 3; i++){
        tri[i] = MakeEqualTriangle(1.0f, 0xffffffff);
        tri[i].translate += Vector3(2.0f, 0.0f, 0.0f) * float(2 - (i + 1));
    }

    GH.push_back(SEED::LoadTexture("resources/textures/uvChecker.png"));
    GH.push_back(SEED::LoadTexture("resources/textures/symmetryORE1.png"));
    GH.push_back(SEED::LoadTexture("resources/textures/symmetryORE2.png"));
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

    ImGui::Begin("triangle");
    ImGui::InputInt("GH", &num, 0, int(GH.size()) - 1);
    ImGui::End();

    for(int i = 0; i < 3; i++){
        SEED::DrawTriangleTex(tri[i], GH[i]);
        tri[i].rotate.y += 0.01f;
    }
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
