#pragma once
#include "DxManager.h"
#include "ImGuiManager.h"
#include "WindowManager.h"
#include "Triangle.h"
#include "Camera.h"
#include "PolygonManager.h"

class SEED{

public:// 基本関数
    SEED();
    SEED(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);
    ~SEED();
    static void Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);
    static void Finalize();

public:// 基本関数以外ででかめの関数
    static void BeginFrame();
    static void EndFrame();

public:
    /////////////////////////////////////////////////////////////////////////////////////
    /*                             テクスチャを読み込む関数                                */
    /////////////////////////////////////////////////////////////////////////////////////

    // 返り値はグラフハンドル
    uint32_t LoadTexture(const std::string& filePath)const;


public:// このエンジンが用意する描画関数

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                三角形を描く関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

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

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                モデルの描画登録                                    */
    /////////////////////////////////////////////////////////////////////////////////////

    static void DrawModel(const Model& model, uint32_t textureGH);


private:// マネージャたち
    static std::unique_ptr<WindowManager> windowManager_;
    static std::unique_ptr<DxManager> dxManager_;
    static std::unique_ptr<ImGuiManager> imguiManager_;

public:// ウインドウに関する変数
    static HWND hwnd;
    static HINSTANCE hInstance_;
    static int nCmdShow_;
    static MSG msg_;

    static std::string windowTitle_;
    static int kClientWidth_;
    static int kClientHeight_;

public:
    static SEED* pSEED_;
    static PolygonManager* pPolygonManager_;

public: // アクセッサ
    static void SetPolygonManagerPtr(PolygonManager* ptr){ pPolygonManager_ = ptr; }
    static DxManager* GetDxManager(){ return dxManager_.get(); }
    static Camera* GetCamera(){ return dxManager_->GetCamera(); }
};