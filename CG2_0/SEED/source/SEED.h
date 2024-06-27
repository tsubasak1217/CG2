#pragma once
#include <mutex>
#include "DxManager.h"
#include "ImGuiManager.h"
#include "WindowManager.h"
#include "Triangle.h"
#include "Camera.h"
#include "PolygonManager.h"

class SEED{

private:// 基本関数
    SEED();

public:
    ~SEED();
    static void Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);
    static void Finalize();
    static SEED* GetInstance();

private:// インスタンス
    static SEED* instance_;
    // 複製禁止
    SEED(const SEED& other) = delete;
    SEED& operator=(const SEED&) = delete;

public:// 基本関数以外ででかめの関数
    static void BeginFrame();
    static void EndFrame();

public:
    /////////////////////////////////////////////////////////////////////////////////////
    /*                             テクスチャを読み込む関数                                */
    /////////////////////////////////////////////////////////////////////////////////////

    // 返り値はグラフハンドル
    static uint32_t LoadTexture(const std::string& filePath);


public:// このエンジンが用意する描画関数

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                三角形を描く関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

    static void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color,uint32_t GH = 0);
    static void DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color,uint32_t GH = 0);
    static void DrawTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color, uint32_t GH = 0
    );
    static void DrawTriangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color,uint32_t GH = 0
    );
    static void DrawTriangle(const Triangle& triangle, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle(const Triangle& triangle, uint32_t GH = 0);

    // 2D-----------------------------------
    static void DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle2D(const Triangle2D& triangle, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle2D(const Triangle2D& triangle, uint32_t GH = 0);

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                モデルの描画登録                                    */
    /////////////////////////////////////////////////////////////////////////////////////

    static void DrawModel(const Model& model);


private:// マネージャたち
    LeakChecker* leakChecker_ = nullptr;
    WindowManager* windowManager_ = nullptr;
    DxManager* dxManager_ = nullptr;
    ImGuiManager* imguiManager_ = nullptr;

public:// ウインドウに関する変数
    HWND hwnd;
    HINSTANCE hInstance_;
    int nCmdShow_;
    MSG msg_;

    std::string windowTitle_;
    int kClientWidth_;
    int kClientHeight_;

private:
    PolygonManager* pPolygonManager_;

public: // アクセッサ
    static void SetPolygonManagerPtr(PolygonManager* ptr){ instance_->pPolygonManager_ = ptr; }
    static DxManager* GetDxManager(){ return instance_->dxManager_; }
    static Camera* GetCamera(){ return instance_->dxManager_->GetCamera(); }
};