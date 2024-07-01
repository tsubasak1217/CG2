#pragma once
#include <string>
#include <vector>
#include "Triangle.h"
#include "Model.h"

class Scene_Game;
class SEED;

struct Particle{
    Triangle2D triangle;
    float radius;
    int lifetime;
    int kLifetime;
    float rotateRate;
};

class GameState_Base{
public:
    GameState_Base(const std::string& name, Scene_Game* pScene) : name_(name), pScene_(pScene){};
    virtual ~GameState_Base(){};
    void ShiftState(GameState_Base* nextState);
    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:
    std::string name_;
    Scene_Game* pScene_ = nullptr;
    SEED* pSEED_ = nullptr;
};

class GameState_Test : public GameState_Base{
public:
    GameState_Test(const std::string& name, Scene_Game* pScene);
    ~GameState_Test();
    void Update()override;
    void Draw()override;

private:
    Triangle tri[8];
    std::vector<uint32_t>GH;
    Model model;
    float h = 0.0f;

    float resolutionRate_ = 1.0f;
    float preRate_ = resolutionRate_;
};

class GameState_Effect : public GameState_Base{
public:
    GameState_Effect(const std::string& name, Scene_Game* pScene);
    ~GameState_Effect();
    void Update()override;
    void Draw()override;
};