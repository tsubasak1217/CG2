#pragma once
#include <string>
#include <vector>
#include "Triangle.h"

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
    Scene_Game* pScene_;
    SEED* pSEED_;
};

class GameState_Test : public GameState_Base{
public:
    GameState_Test(const std::string& name, Scene_Game* pScene);
    ~GameState_Test();
    void Update()override;
    void Draw()override;

private:
    Triangle triangle_[2];
    float rotateRate_;

    std::vector<Particle>tri2D_;
    Vector3 vertices_[4];

    const uint32_t colors[5] = {
        0xffff00ff,
        0x00ffffff,
        0xff00ffff,
        0xa0ff33ff,
        0xff33cfff
    };

    int frameCount = 0;
};

class GameState_Effect : public GameState_Base{
public:
    GameState_Effect(const std::string& name, Scene_Game* pScene);
    ~GameState_Effect();
    void Update()override;
    void Draw()override;
};