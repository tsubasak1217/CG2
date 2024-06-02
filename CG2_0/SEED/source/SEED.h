#pragma once
#include "DxManager.h"
#include "ImGuiManager.h"
#include "WindowManager.h"
#include "Triangle.h"
#include "Camera.h"

class SceneManager;

class SEED{

public:// 基本関数
    SEED();
    SEED(HINSTANCE hInstance, int nCmdShow,const char* windowTitle,int clientWidth,int clientHeight);
    ~SEED();
    static void Initialize(SEED* pSEED);
    static void Finalize();
    static void Run(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);

private:// 基本関数以外ででかめの関数
    static void BeginFrame();
    static void EndFrame();

public:// このエンジンが用意する描画関数
    static void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color);
    static void DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color);
    static void DrawTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color
    );
    static void DrawTriangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color
    );
    static void DrawTriangle(const Triangle& triangle, const Vector4& color);
    static void DrawTriangle(const Triangle& triangle);
    static void DrawTriangleTex(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color);
    static void DrawTriangleTex(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color);
    static void DrawTriangleTex(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color
    );
    static void DrawTriangleTex(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color
    );
    static void DrawTriangleTex(const Triangle& triangle, const Vector4& color);
    static void DrawTriangleTex(const Triangle& triangle);

    // 2D-----------------------------------
    static void DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color);
    static void DrawTriangle2D(const Triangle2D& triangle, const Vector4& color);
    static void DrawTriangle2D(const Triangle2D& triangle);
    static void DrawTriangleTex2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color);
    static void DrawTriangleTex2D(const Triangle2D& triangle, const Vector4& color);
    static void DrawTriangleTex2D(const Triangle2D& triangle);

private:// マネージャたち
    static std::unique_ptr<WindowManager> windowManager_;
    static std::unique_ptr<DxManager> dxManager_;
    static std::unique_ptr<ImGuiManager> imguiManager_;
    static std::unique_ptr<SceneManager> sceneManager_;

public:// ウインドウに関する変数
    static HWND hwnd;
    static HINSTANCE hInstance_;
    static int nCmdShow_;
    static MSG msg_;

    static std::string windowTitle_;
    static int kClientWidth_;
    static int kClientHeight_;

public: // アクセッサ
    static DxManager* GetDxManager(){ return dxManager_.get(); }
    static Camera* GetCamera(){ return dxManager_->GetCamera(); }
};