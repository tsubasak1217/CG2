#pragma once
#include <mutex>
#include "DxManager.h"
#include "ImGuiManager.h"
#include "WindowManager.h"
#include "Triangle.h"
#include "Camera.h"
#include "PolygonManager.h"

class SEED{

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     基本の関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
private:
    SEED(){};// singletonのためにprivateに置く
    SEED(const SEED& other) = delete;
    SEED& operator=(const SEED&) = delete;

public:
    ~SEED();
    static void Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);
    static void Finalize();
    static SEED* GetInstance();
    static void BeginFrame();
    static void EndFrame();


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 いろーんな関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    // テクスチャを読み込む関数(返り値はグラフハンドル)
    static uint32_t LoadTexture(const std::string& filePath);
    // 画面の解像度を変更する関数(0.0f ~ 1.0f)
    static void ChangeResolutionRate(float resolutionRate);


    /////////////////////////////////////////////////////////////////////////////////////
    /*                          このエンジンが用意する描画関数                              */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    /*==========================三角形の描画関数==========================*/

    static void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color, uint32_t GH = 0
    );
    static void DrawTriangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color, uint32_t GH = 0
    );
    static void DrawTriangle(const Triangle& triangle, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle(const Triangle& triangle, uint32_t GH = 0);
    static void DrawTriangle2D(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle2D(const Triangle2D& triangle, const Vector4& color, uint32_t GH = 0);
    static void DrawTriangle2D(const Triangle2D& triangle, uint32_t GH = 0);


    /*==========================モデルの描画関数==========================*/

    static void DrawModel(const Model& model);


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     メンバ変数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

private:// インスタンス

    static SEED* instance_;

private:// マネージャたち

    LeakChecker* leakChecker_ = nullptr;
    WindowManager* windowManager_ = nullptr;
    DxManager* dxManager_ = nullptr;
    ImGuiManager* imguiManager_ = nullptr;

private:// ウインドウに関する変数

    static HWND hwnd;
    static HINSTANCE hInstance_;
    static int nCmdShow_;
    static MSG msg_;

public:
    int kClientWidth_;
    int kClientHeight_;
    std::string windowTitle_;
    uint32_t windowBackColor_;

private:// 外部を参照するためのポインタ変数

    PolygonManager* pPolygonManager_;


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 アクセッサ関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    static void SetPolygonManagerPtr(PolygonManager* ptr){ instance_->pPolygonManager_ = ptr; }
    static DxManager* GetDxManager(){ return instance_->dxManager_; }
    static Camera* GetCamera(){ return GetInstance()->dxManager_->GetCamera(); }

    static HWND GetHWND(){ return hwnd; }
    static void SetWindowHandle(HWND handle){ hwnd = handle; }
    static int GetCmdShow(){ return nCmdShow_; }
    static HINSTANCE GetHINSTANCE(){ return hInstance_; }
    static UINT ProcessMessage(){ return msg_.message; }
    static void SetWindowColor(uint32_t color){ GetInstance()->windowBackColor_ = color; }
    static uint32_t GetWindowColor(){ return GetInstance()->windowBackColor_; }
};