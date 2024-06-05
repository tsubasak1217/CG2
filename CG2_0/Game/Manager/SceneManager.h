#pragma once
#include "Scene_Base.h"
#include "Scene_Game.h"
#include <memory>
class SEED;

class SceneManager{
public:
    SceneManager(SEED* pSEED);
    ~SceneManager();
    void Update();
    void Draw();

public:
    void ChangeScene(Scene_Base* newScene);

private:
    std::unique_ptr<Scene_Base> pScene_;
    SEED* pSEED_;
};