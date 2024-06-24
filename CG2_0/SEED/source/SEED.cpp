#include "SEED.h"
#include "DxFunc.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "includes.h"
#include "Environment.h"
#include "SceneManager.h"

/*----------------------------------- static変数の初期化----------------------------------------*/

std::unique_ptr<WindowManager> SEED::windowManager_ = nullptr;
std::unique_ptr<DxManager> SEED::dxManager_ = nullptr;
std::unique_ptr<ImGuiManager> SEED::imguiManager_ = nullptr;

HWND SEED::hwnd = nullptr;
HINSTANCE SEED::hInstance_ = nullptr;
int SEED::nCmdShow_ = 0;
MSG SEED::msg_ = {};
std::string SEED::windowTitle_ = "";
int SEED::kClientWidth_ = 0;
int SEED::kClientHeight_ = 0;

std::unique_ptr <SEED> SEED::pSEED_ = nullptr;
PolygonManager* SEED::pPolygonManager_ = nullptr;

/*---------------------------------コンストラクタ、デストラクタ------------------------------------*/

SEED::SEED(){}

SEED::SEED(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight)
{
    hInstance_ = hInstance;
    nCmdShow_ = nCmdShow;
    windowTitle_ = windowTitle;
    kClientWidth_ = clientWidth;
    kClientHeight_ = clientHeight;

    windowManager_ = std::make_unique<WindowManager>();
    dxManager_ = std::make_unique<DxManager>();
    imguiManager_ = std::make_unique<ImGuiManager>();

    pSEED_.reset(this);
}

SEED::~SEED(){}

/*-------------------------------------- 初期化、終了処理 -----------------------------------------*/

void SEED::Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight){
    SEED seed(hInstance, nCmdShow, windowTitle, clientWidth, clientHeight);
    pSEED_->windowManager_->Initialize(pSEED_.get());
    pSEED_->dxManager_->Initialize(pSEED_.get());
    pSEED_->imguiManager_->Initialize(pSEED_.get());
}

void SEED::Finalize()
{
    pSEED_->imguiManager_->Finalize();
    pSEED_->windowManager_->Finalize();
    pSEED_->dxManager_->Finalize();
}

/*------------------------------------ ゲームエンジン実行部分 --------------------------------------*/

//void SEED::Run(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight)
//{
//    SEED::Initialize(hInstance, nCmdShow, windowTitle, clientWidth, clientHeight);
//
//    while(msg_.message != WM_QUIT){
//        SEED::BeginFrame();
//
//        pSEED_->sceneManager_->Update();
//        pSEED_->sceneManager_->Draw();
//
//        SEED::EndFrame();
//    }
//
//    SEED::Finalize();
//}

/*-------------------------------------- ゲームループ内の関数 -----------------------------------------*/

void SEED::BeginFrame()
{
    // ウインドウにメッセージがある場合、優先して処理する
    if(PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);
    }

    pSEED_->imguiManager_->Begin();
    pSEED_->dxManager_->PreDraw();
}

void SEED::EndFrame()
{
    pSEED_->dxManager_->DrawPolygonAll();
    pSEED_->imguiManager_->End();
    pSEED_->dxManager_->PostDraw();
}

/////////////////////////////////////////////////////////////////////////////////////
/*                             テクスチャを読み込む関数                                */
/////////////////////////////////////////////////////////////////////////////////////

uint32_t SEED::LoadTexture(const std::string& filePath) const
{
    return pSEED_->dxManager_.get()->CreateTexture(filePath);
}

/*--------------------------------------------------------------------------------------------------*/
/*                                            描画関数                                               */
/*--------------------------------------------------------------------------------------------------*/

/*=============================================== 3D ===========================================*/

void SEED::DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color)
{
    pSEED_->dxManager_->DrawTriangle(v1, v2, v3, IdentityMat4(), color, false, true,0);
}

void SEED::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color)
{
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3), IdentityMat4(),
        color, false, true,0
    );
}

void SEED::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    pSEED_->dxManager_->DrawTriangle(v1, v2, v3, worldMat, color, false, true,0);
}

void SEED::DrawTriangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3),
        worldMat, color, false, true,0
    );
}

void SEED::DrawTriangle(const Triangle& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, false, true,0
    );
}

void SEED::DrawTriangle(const Triangle& triangle)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, true,0
    );
}

/*----------------------------------Tex------------------------------------*/

void SEED::DrawTriangleTex(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color, uint32_t GH)
{
    pSEED_->dxManager_->DrawTriangle(v1, v2, v3, IdentityMat4(), color, true, true,GH);
}

void SEED::DrawTriangleTex(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color, uint32_t GH)
{
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3), IdentityMat4(),
        color, true, true,GH
    );
}

void SEED::DrawTriangleTex(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    pSEED_->dxManager_->DrawTriangle(v1, v2, v3, worldMat, color, true, true,GH);
}

void SEED::DrawTriangleTex(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3),
        worldMat, color, true, true,GH
    );
}

void SEED::DrawTriangleTex(const Triangle& triangle, const Vector4& color, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, true, true,GH
    );
}

void SEED::DrawTriangleTex(const Triangle& triangle, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, true, true,GH
    );
}

/*=============================================== 2D ===========================================*/

void SEED::DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color)
{
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1),
        TransformToVec4(v2),
        TransformToVec4(v3),
        IdentityMat4(), color, false, false,0
    );
}

void SEED::DrawTriangle2D(const Triangle2D& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, false, false,0
    );
}

void SEED::DrawTriangle2D(const Triangle2D& triangle)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, false,0
    );
}

/*----------------------------------Tex------------------------------------*/

void SEED::DrawTriangleTex2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color, uint32_t GH)
{
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(v1),
        TransformToVec4(v2),
        TransformToVec4(v3),
        IdentityMat4(), color, true, false,GH
    );
}

void SEED::DrawTriangleTex2D(const Triangle2D& triangle, const Vector4& color, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, true, false,GH
    );
}

void SEED::DrawTriangleTex2D(const Triangle2D& triangle, uint32_t GH)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    pSEED_->dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, true, false,GH
    );
}

void SEED::DrawModel(const Model& model, uint32_t textureGH){
    pSEED_->pPolygonManager_->AddModel(model,textureGH);
}
