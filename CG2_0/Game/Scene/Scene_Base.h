#pragma once
class SceneManager;

class Scene_Base{
public:
    Scene_Base() = default;
    Scene_Base(SceneManager* pSceneManager);
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:
    SceneManager* pSceneManager_;
};