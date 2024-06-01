#pragma once

class DxManager;

struct PSOManager{

    PSOManager(DxManager* pDxManager);
    ~PSOManager();
    void Create();

private:
    DxManager* pDxManager_;
};