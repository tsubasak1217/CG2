#pragma once
#include "Scene_Base.h"
#include "GameState.h"
#include <memory>

class Scene_Game : public Scene_Base{
public:
    Scene_Game(SceneManager* pSceneManager, SEED* pSEED);
    ~Scene_Game()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

public:
    void ChangeState(GameState_Base* newState);

protected:

private:
    std::unique_ptr<GameState_Base> gameState_;
};