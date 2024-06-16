#pragma once
#include "SEED.h"
#include "Scene_Base.h"
#include "Scene_Game.h"
#include <memory>
class SEED;

class SceneManager{
public:
    SceneManager();
    ~SceneManager();
    static void Initialize();
    static void Update();
    static void Draw();

public:
    static void ChangeScene(Scene_Base* newScene);

private:
    static std::unique_ptr<SceneManager> pSceneManager_;
    static std::unique_ptr<Scene_Base> pScene_;
};