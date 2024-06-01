#include "WindowManager.h"
#include "SEED.h"
#include "DxManager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

void WindowManager::Initialize(SEED* pSEED)
{
    pSEED_ = pSEED;

    /*===========================================================================================*/
    /*                                   ウインドウの初期化                                         */
    /*===========================================================================================*/

     /*------------------------------ クライアント領域の決定 ------------------------------*/

    const int32_t kClientWidth = pSEED_->kClientWidth_;
    const int32_t kClientHeight = pSEED_->kClientHeight_;
    RECT rect = { 0,0,kClientWidth,kClientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    /*------------------------------ ウインドウクラスの登録 ------------------------------*/

    WNDCLASS wc{};// 空のウインドウクラス
    TCHAR szAppName[] = L"test";// ウィンドウクラスの名前

    // ウインドウクラスの中身を記述
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = pSEED_->hInstance_;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szAppName;

    if(!RegisterClass(&wc)) { return; }

    /*-------------------------------- ウィンドウの生成 ---------------------------------*/

    pSEED_->hwnd = CreateWindow(
        szAppName,
        ConvertString(pSEED_->windowTitle_).c_str(),
        WS_OVERLAPPEDWINDOW,
        //ウィンドウ左上座標
        CW_USEDEFAULT, CW_USEDEFAULT,
        //ウィンドウ幅と高さ
        rect.right - rect.left,
        rect.bottom - rect.top,
        //
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if(!pSEED_->hwnd) { return; }


    // ウインドウの表示
    ShowWindow(pSEED_->hwnd, pSEED_->nCmdShow_);

    // ウインドウの更新
    UpdateWindow(pSEED_->hwnd);
}

void WindowManager::Finalize()
{
}

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if(ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
        return true;
    }

    switch(uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        // 標準のメッセージ処理を行う
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}