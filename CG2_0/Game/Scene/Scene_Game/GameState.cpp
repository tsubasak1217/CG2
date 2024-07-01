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
    for(int i = 0; i < 8; i++){
        tri[i] = MakeEqualTriangle(1.0f, 0xffffffff);
        tri[i].rotate.y = 3.14f * (i / 4);
    }

    tri[0].translate = { -2.0f,1.5f,0.0f };
    tri[1].translate = { 2.0f,1.5f,0.0f };
    tri[2].translate = { -2.0f,-1.5f,0.0f };
    tri[3].translate = { 2.0f,-1.5f,0.0f };

    tri[4].translate = { 0.0f,1.5f,0.0f };
    tri[5].translate = { 0.0f,-1.5f,0.0f };
    tri[6].translate = { -2.0f,0.0f,0.0f };
    tri[7].translate = { 2.0f,0.0f,0.0f };


    GH.push_back(SEED::LoadTexture("resources/textures/uvChecker.png"));
    GH.push_back(SEED::LoadTexture("resources/textures/symmetryORE1.png"));
    GH.push_back(SEED::LoadTexture("resources/textures/symmetryORE2.png"));

    model.Initialize("resources/models", "monkey.obj");
}

GameState_Test::~GameState_Test()
{
}

void GameState_Test::Update()
{
    /*======================= 前フレームの値保存 ======================*/

    preRate_ = resolutionRate_;

    /*========================== ImGui =============================*/

    ImGui::Begin("TR2");

    ImGui::Text("CAMERA");
    ImGui::DragFloat3("translate", &SEED::GetCamera()->transform_.translate_.x, 0.05f);

    ImGui::Text("RESOLUTION_RATE");
    ImGui::SliderFloat("rate", &resolutionRate_, 0.0f,1.0f);

    ImGui::End();

    /*===============================================================*/
    
    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    SEED::GetCamera()->Update();
}

void GameState_Test::Draw()
{

    for(int i = 0; i < 8; i++){
        SEED::DrawTriangle(tri[i], { 1.0f,1.0f,1.0f,1.0f }, GH[1 + (i / 4)]);
        tri[i].rotate.y += 0.01f;
    }

    SEED::DrawModel(model);
    model.SetRotate(tri[4].rotate);
    model.UpdateMatrix();

    h += 0.0025f;
    SEED::SetWindowColor(MyMath::HSV_to_RGB(h, 1.0f, 0.6f, 1.0f));
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
