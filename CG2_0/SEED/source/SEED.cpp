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
std::unique_ptr<SceneManager> SEED::sceneManager_ = nullptr;

HWND SEED::hwnd = nullptr;
HINSTANCE SEED::hInstance_ = nullptr;
int SEED::nCmdShow_ = 0;
MSG SEED::msg_ = {};
std::string SEED::windowTitle_ = "";
int SEED::kClientWidth_ = 0;
int SEED::kClientHeight_ = 0;

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
    sceneManager_ = std::make_unique<SceneManager>(this);
}

SEED::~SEED(){}

/*-------------------------------------- 初期化、終了処理 -----------------------------------------*/

void SEED::Initialize(SEED* pSEED){
    windowManager_->Initialize(pSEED);
    dxManager_->Initialize(pSEED);
    imguiManager_->Initialize(pSEED);
}

void SEED::Finalize()
{
    imguiManager_->Finalize();
    windowManager_->Finalize();
    dxManager_->Finalize();
}

/*------------------------------------ ゲームエンジン実行部分 --------------------------------------*/

void SEED::Run(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight)
{
    SEED seed(hInstance, nCmdShow, windowTitle, clientWidth, clientHeight);

    Initialize(&seed);

    while(msg_.message != WM_QUIT){
        BeginFrame();

        sceneManager_->Update();
        sceneManager_->Draw();

        EndFrame();
    }

    Finalize();
}

/*-------------------------------------- ゲームループ内の関数 -----------------------------------------*/

void SEED::BeginFrame()
{
    // ウインドウにメッセージがある場合、優先して処理する
    if(PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);
    }

    imguiManager_->Begin();
    dxManager_->PreDraw();
}

void SEED::EndFrame()
{
    dxManager_->DrawPolygonAll();
    imguiManager_->End();
    dxManager_->PostDraw();
}

/*--------------------------------------------------------------------------------------------------*/
/*                                            描画関数                                               */
/*--------------------------------------------------------------------------------------------------*/

/*=============================================== 3D ===========================================*/

void SEED::DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(v1, v2, v3, IdentityMat4(), color, false, true);
}

void SEED::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3), IdentityMat4(),
        color, false, true
    );
}

void SEED::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(v1, v2, v3, worldMat, color, false, true);
}

void SEED::DrawTriangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3),
        worldMat, color, false, true
    );
}

void SEED::DrawTriangle(const Triangle& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, false, true
    );
}

void SEED::DrawTriangle(const Triangle& triangle)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, true
    );
}

/*----------------------------------Tex------------------------------------*/

void SEED::DrawTriangleTex(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(v1, v2, v3, IdentityMat4(), color, true, true);
}

void SEED::DrawTriangleTex(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3), IdentityMat4(),
        color, true, true
    );
}

void SEED::DrawTriangleTex(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(v1, v2, v3, worldMat, color, true, true);
}

void SEED::DrawTriangleTex(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(
        TransformToVec4(v1), TransformToVec4(v2), TransformToVec4(v3),
        worldMat, color, true, true
    );
}

void SEED::DrawTriangleTex(const Triangle& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, true, true
    );
}

void SEED::DrawTriangleTex(const Triangle& triangle)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, true, true
    );
}

/*=============================================== 2D ===========================================*/

void SEED::DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(
        TransformToVec4(v1),
        TransformToVec4(v2),
        TransformToVec4(v3),
        IdentityMat4(), color, false, false
    );
}

void SEED::DrawTriangle2D(const Triangle2D& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, false, false
    );
}

void SEED::DrawTriangle2D(const Triangle2D& triangle)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, false, false
    );
}

/*----------------------------------Tex------------------------------------*/

void SEED::DrawTriangleTex2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color)
{
    dxManager_->DrawTriangle(
        TransformToVec4(v1),
        TransformToVec4(v2),
        TransformToVec4(v3),
        IdentityMat4(), color, true, false
    );
}

void SEED::DrawTriangleTex2D(const Triangle2D& triangle, const Vector4& color)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, color, true, false
    );
}

void SEED::DrawTriangleTex2D(const Triangle2D& triangle)
{
    Matrix4x4 worldMat = AffineMatrix({ triangle.scale.x,triangle.scale.y,1.0f }, { 0.0f, 0.0f, triangle.rotate }, { triangle.translate.x,triangle.translate.y,0.0f });
    dxManager_->DrawTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.colorf, true, false
    );
}
