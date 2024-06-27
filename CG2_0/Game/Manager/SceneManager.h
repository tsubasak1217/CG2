#pragma once
#include "SEED.h"
#include "Scene_Base.h"
#include "Scene_Game.h"
#include <memory>
class SEED;

class SceneManager{

private:
    SceneManager();
public:
    ~SceneManager();
    static void Initialize();
    static void Update();
    static void Draw();
    static SceneManager* GetInstance();

private:
    // インスタンス
    static SceneManager* instance_;

    // コピー禁止
    SceneManager(const SceneManager& other) = delete;
    SceneManager& operator=(const SceneManager& other) = delete;

public:
    static void ChangeScene(Scene_Base* newScene);

private:
    static Scene_Base* pScene_;
};