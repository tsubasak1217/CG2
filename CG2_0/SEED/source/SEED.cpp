#include "SEED.h"
#include "DxFunc.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "includes.h"

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

/*---------------------------------コンストラクタ、デストラクタ------------------------------------*/

SEED::SEED(){}

SEED::SEED(HINSTANCE hInstance, int nCmdShow, const char* kWindowTitle, int clientWidth, int clientHeight)
{
    hInstance_ = hInstance;
    nCmdShow_ = nCmdShow;
    windowTitle_ = kWindowTitle;
    kClientWidth_ = clientWidth;
    kClientHeight_ = clientHeight;

    windowManager_ = std::make_unique<WindowManager>();
    dxManager_ = std::make_unique<DxManager>();
    imguiManager_ = std::make_unique<ImGuiManager>();
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

void SEED::Run(HINSTANCE hInstance, int nCmdShow, const char* kWindowTitle, int clientWidth, int clientHeight)
{
    SEED seed(hInstance, nCmdShow, kWindowTitle, clientWidth, clientHeight);

    Initialize(&seed);

    Triangle triangle[2] = {
        Triangle(
         { 0.0f, 0.5f, 0.0f},
         { 0.5f, -0.5f, 0.0f},
         { -0.5f, -0.5f, 0.0f}
         ),

        Triangle(
         { -0.5f, 0.5f, 0.0f},
         { 0.5f, 0.5f, 0.0f},
         { 0.0f, -0.5f, 0.0f}
        )
    };

    triangle[0].color = 0xffffffff;
    triangle[1].color = 0x00ffffff;

    while(msg_.message != WM_QUIT){
        BeginFrame();

        ImGui::ShowDemoWindow();

        triangle[0].rotate.y += 3.14f * 0.005f;
        triangle[1].rotate.z += 3.14f * 0.005f;

        for(int i = 0; i < 1; i++){
            DrawTriangle(triangle[i]);
        }

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

/*------------------------------------------- 描画関数 ----------------------------------------------*/

void SEED::DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, uint32_t color)
{
    dxManager_->DrawTriangle(v1, v2, v3, IdentityMat4(), color);
}

void SEED::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, uint32_t color)
{
    dxManager_->DrawTriangle(TransformToVector4(v1), TransformToVector4(v2), TransformToVector4(v3), IdentityMat4(), color);
}

void SEED::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    uint32_t color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(v1, v2, v3, worldMat, color);
}

void SEED::DrawTriangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    uint32_t color
){
    Matrix4x4 worldMat = AffineMatrix(scale, rotate, translate);
    dxManager_->DrawTriangle(TransformToVector4(v1), TransformToVector4(v2), TransformToVector4(v3), worldMat, color);
}

void SEED::DrawTriangle(const Triangle& triangle, uint32_t color)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVector4(triangle.localVertex[0]),
        TransformToVector4(triangle.localVertex[1]),
        TransformToVector4(triangle.localVertex[2]),
        worldMat, color
    );
}

void SEED::DrawTriangle(const Triangle& triangle)
{
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    dxManager_->DrawTriangle(
        TransformToVector4(triangle.localVertex[0]),
        TransformToVector4(triangle.localVertex[1]),
        TransformToVector4(triangle.localVertex[2]),
        worldMat, triangle.color
    );
}
