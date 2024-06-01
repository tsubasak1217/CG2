#pragma once
#include "DxManager.h"

class SEED;

class ImGuiManager{
public:
    void Initialize(SEED* pSEED);
    void Finalize();

    void Draw();

    void Begin();
    void End();

private:
    SEED* pSEED_;
    DxManager* pDxManager_;
};