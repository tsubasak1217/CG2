#pragma once
class SceneManager;
class SEED;

class Scene_Base{
public:
    Scene_Base() = default;
    Scene_Base(SceneManager* pSceneManager,SEED* pSEED);
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:
    SceneManager* pSceneManager_;

public:
    SEED* pSEED_;
};