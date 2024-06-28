#include "SEED.h"
#include "DxFunc.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "includes.h"
#include "Environment.h"
#include "SceneManager.h"

/*----------------------------------- static変数の初期化----------------------------------------*/

//WindowManager* SEED::windowManager_ = nullptr;
//DxManager* SEED::dxManager_ = nullptr;
//ImGuiManager* SEED::imguiManager_ = nullptr;
//
HWND SEED::hwnd = nullptr;
HINSTANCE SEED::hInstance_ = nullptr;
int SEED::nCmdShow_ = 0;
MSG SEED::msg_ = {};
//std::string SEED::windowTitle_ = "";
//int SEED::kClientWidth_ = 0;
//int SEED::kClientHeight_ = 0;
//
//PolygonManager* SEED::pPolygonManager_ = nullptr;

SEED* SEED::instance_ = nullptr;

/*---------------------------------コンストラクタ、デストラクタ------------------------------------*/

SEED::SEED(){}
SEED::~SEED(){
    delete imguiManager_;
    imguiManager_ = nullptr;
    delete dxManager_;
    dxManager_ = nullptr;
    delete windowManager_;
    windowManager_ = nullptr;
    delete leakChecker_;
    leakChecker_ = nullptr;
    delete instance_;
    instance_ = nullptr;
}

/*-------------------------------------- 初期化、終了処理 -----------------------------------------*/

void SEED::Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight){

    instance_ = GetInstance();

    instance_->hInstance_ = hInstance;
    instance_->nCmdShow_ = nCmdShow;
    instance_->windowTitle_ = windowTitle;
    instance_->kClientWidth_ = clientWidth;
    instance_->kClientHeight_ = clientHeight;

    instance_->leakChecker_ = new LeakChecker();
    instance_->windowManager_ = new WindowManager();
    instance_->dxManager_ = new DxManager();
    instance_->imguiManager_ = new ImGuiManager();

    instance_->windowManager_->Initialize(instance_);
    instance_->dxManager_->Initialize(instance_);
    instance_->imguiManager_->Initialize(instance_);
}

void SEED::Finalize()
{
    instance_->imguiManager_->Finalize();
    instance_->windowManager_->Finalize();
    instance_->dxManager_->Finalize();
}

SEED* SEED::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(
        onceFlag,
        [](){
        if(!instance_){ instance_ = new SEED; }
    });

    return instance_;
}

/*------------------------------------ ゲームエンジン実行部分 --------------------------------------*/

/*-------------------------------------- ゲームループ内の関数 -----------------------------------------*/

void SEED::BeginFrame()
{
    // ウインドウにメッセージがある場合、優先して処理する
    if(PeekMessage(&instance_->msg_, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&instance_->msg_);
        DispatchMessage(&instance_->msg_);
    }

    instance_->imguiManager_->Begin();
    instance_->dxManager_->PreDraw();
}

void SEED::EndFrame()
{
    instance_->dxManager_->DrawPolygonAll();
    instance_->imguiManager_->End();
    instance_->dxManager_->PostDraw();
}

/////////////////////////////////////////////////////////////////////////////////////
/*                             テクスチャを読み込む関数                                */
/////////////////////////////////////////////////////////////////////////////////////

uint32_t SEED::LoadTexture(const std::string& filePath)
{
    return instance_->dxManager_->CreateTexture(filePath);
}

/*--------------------------------------------------------------------------------------------------*/
/*                                            描画関数                                               */
/*--------------------------------------------------------------------------------------------------*/

/*=============================================== 3D ===========================================*/

void SEED::DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color, uint32_t GH)
{
    Triangle tri(TransformToVec3(v1), TransformToVec3(v2), TransformToVec3(v3));
    tri.colorf = color;
    SEED::DrawTriangle(tri, GH);
}

void SEED::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color, uint32_t GH)
{
    Triangle tri(v1, v2, v3);
    tri.colorf = color;
    SEED::DrawTriangle(tri, GH);
}

void SEED::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH
){
    Triangle tri(
        TransformToVec3(v1), TransformToVec3(v2), TransformToVec3(v3),
        scale, rotate, translate
    );

    tri.colorf = color;
    SEED::DrawTriangle(tri, GH);
}

void SEED::DrawTriangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH
){
    Triangle tri(
        v1, v2, v3, scale, rotate, translate
    );

    tri.colorf = color;
    SEED::DrawTriangle(tri, GH);
}

void SEED::DrawTriangle(const Triangle& triangle, const Vector4& color, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    instance_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, triangle.enableLiting_, triangle.uvTransform_, true, GH
    );
}

void SEED::DrawTriangle(const Triangle& triangle, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    instance_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, triangle.uvTransform_, true, GH
    );
}

/*=============================================== 2D ===========================================*/

void SEED::DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color, uint32_t GH)
{
    Triangle2D tri(v1, v2, v3);
    tri.colorf = color;
    SEED::DrawTriangle2D(tri, GH);
}

void SEED::DrawTriangle2D(const Triangle2D& triangle, const Vector4& color, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    instance_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, false, triangle.uvTransform_, false, GH
    );
}

void SEED::DrawTriangle2D(const Triangle2D& triangle, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    instance_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, triangle.uvTransform_, false, GH
    );
}


//--------------------------------------------------------------
void SEED::DrawModel(const Model& model){
    instance_->pPolygonManager_->AddModel(model);
}
