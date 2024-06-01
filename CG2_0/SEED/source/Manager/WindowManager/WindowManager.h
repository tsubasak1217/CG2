#pragma once

class SEED;

class WindowManager{
public:
    void Initialize(SEED* pSEED);
    void Finalize();

private:
    SEED* pSEED_;
};