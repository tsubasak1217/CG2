#include "Scene_Base.h"
#include "SceneManager.h"

Scene_Base::Scene_Base(SceneManager* pSceneManager,SEED* pSEED)
{
    pSEED_ = pSEED;
    pSceneManager_ = pSceneManager;
}
