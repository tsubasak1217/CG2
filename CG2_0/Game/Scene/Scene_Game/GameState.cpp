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

    pScene_ = pScene;
    pSEED_ = pScene_->pSEED_;

    for(int i = 0; i < 2; i++){
        triangle_[i] = MakeEqualTriangle(1.0f, 0xffffffff);
    }

    triangle_[1].rotate.z += 3.14f;
    rotateRate_ = 3.14f / 120.0f;
}

GameState_Test::~GameState_Test()
{
}

void GameState_Test::Update()
{
    ImGui::Begin("triangle");
    ImGui::Text("COLOR");
    ImGui::ColorEdit4("triangle1##0", &triangle_[0].colorf.x);
    ImGui::ColorEdit4("triangle2##0", &triangle_[1].colorf.x);
    ImGui::Text("SCALE");
    ImGui::DragFloat3("triangle1##1", &triangle_[0].scale.x, 0.05f);
    ImGui::DragFloat3("triangle2##1", &triangle_[1].scale.x, 0.05f);
    ImGui::Text("TRANSLATE");
    ImGui::DragFloat3("triangle1##2", &triangle_[0].translate.x,0.05f);
    ImGui::DragFloat3("triangle2##2", &triangle_[1].translate.x,0.05f);
    ImGui::Text("ROTATE");
    ImGui::DragFloat3("triangle1##3", &triangle_[0].rotate.x, 0.005f);
    ImGui::DragFloat3("triangle2##3", &triangle_[1].rotate.x, 0.005f);
    ImGui::End();

    frameCount++;

    if(frameCount % 3 == 0){
        tri2D_.push_back(Particle());
        tri2D_.back().radius = 5.0f + float(std::rand() % 30);
        tri2D_.back().triangle = MakeEqualTriangle2D(tri2D_.back().radius, colors[std::rand() % 5]);
        tri2D_.back().triangle.translate = { float(std::rand() % 1280),float(std::rand() % 720) };
        tri2D_.back().lifetime = 16 + std::rand() % 60;
        tri2D_.back().kLifetime = tri2D_.back().lifetime;
        tri2D_.back().rotateRate = (std::numbers::pi_v<float> * 0.005f) * float(std::rand() % 10);
    }


    for(int i = 0; i < tri2D_.size(); i++){
        tri2D_[i].triangle.rotate += tri2D_[i].rotateRate;
        tri2D_[i].lifetime--;
        tri2D_[i].triangle.scale = {
            float(tri2D_[i].lifetime) / float(tri2D_[i].kLifetime),
            float(tri2D_[i].lifetime) / float(tri2D_[i].kLifetime)
        };

        if(tri2D_[i].lifetime == 0){
            tri2D_.erase(tri2D_.begin() + i);
        }
    }


}

void GameState_Test::Draw()
{

    for(int i = 0; i < 2; i++){
        SEED::DrawTriangleTex(triangle_[i]);
    }

    for(int i = 0; i < tri2D_.size(); i++){
        SEED::DrawTriangle2D(tri2D_[i].triangle);
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
