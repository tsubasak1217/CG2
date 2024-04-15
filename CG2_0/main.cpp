#include <windows.h>
#include <cstdint>
#include "myFunc.h"
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

LRESULT CALLBACK WndProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE,
    LPSTR, int nCmdShow
) {
   /*===========================================================================================*/
   /*                                   ウインドウの初期化                                         */
   /*===========================================================================================*/

    /*------------------------------ クライアント領域の決定 ------------------------------*/

    const int32_t kCliantWidth = 1280;
    const int32_t kCliantHeight = 720;
    RECT rect = { 0,0,kCliantWidth,kCliantHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    /*------------------------------ ウインドウクラスの登録 ------------------------------*/

    WNDCLASS wc{};// 空のウインドウクラス
    TCHAR szAppName[] = L"test";// ウィンドウ名

    // ウインドウクラスの中身を記述
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szAppName;

    if (!RegisterClass(&wc)) { return 0; }

    /*-------------------------------- ウィンドウの生成 ---------------------------------*/

    HWND hwnd = CreateWindow(
        szAppName, L"CG2",
        WS_OVERLAPPEDWINDOW,
        //ウィンドウ左上座標
        CW_USEDEFAULT, CW_USEDEFAULT,
        //ウィンドウ幅と高さ
        rect.right - rect.left,
        rect.bottom - rect.top,
        //
        nullptr,
        nullptr,
        hInstance, 
        nullptr
    );

    if (!hwnd) { return 0; }
    
    /*===========================================================================================*/
    /*                                   DirextXの初期化                                          */
    /*===========================================================================================*/

    /*------------------------------- DXGIFactoryの生成 --------------------------------*/

    IDXGIFactory7* dxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    // 作成失敗していたらアサート
    assert(SUCCEEDED(hr));

    /*--------------------------- 使用するアダプタ(GPU)の決定 -----------------------------*/

    // アダプタを格納する変数の用意
    IDXGIAdapter4* useAdapter = nullptr;

    // 性能のいいGPUからループが先に来るようfor文を回す
    for (
        int i = 0;
        dxgiFactory->EnumAdapterByGpuPreference(
            UINT(i),
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&useAdapter)
        ) != DXGI_ERROR_NOT_FOUND;
        ++i
        ) {

        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);

        // 取得できない場合アサート
        assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ採用
        if (adapterDesc.Flags != DXGI_ADAPTER_FLAG3_SOFTWARE) {

            // 使用アダプタを出力してループ終了
            Log(std::format(L"useAdapter:{}\n", adapterDesc.Description));
            break;

        }

        // ソフトウェアアダプタだった場合リセットして次のループへ  
        useAdapter = nullptr;
    }

    // アダプタが見つからなかった場合アサート
    assert(useAdapter != nullptr);

    /*------------------------------- D3D12Xデバイスの生成 --------------------------------*/
    
    // 生成したデバイスを格納する変数
    ID3D12Device* device = nullptr;

    // 
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0,
    };

    const char* featureLevelString[] = { "12.2","12.1","12.0" };

    for (size_t i = 0; i < _countof(featureLevels); ++i) {

        // 先ほど決定したアダプタを使用してデバイスを生成
        hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

        // 生成に成功したらログを出力してループを終了
        if (SUCCEEDED(hr)) {
            Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
            break;
        }
    }

    // デバイス生成ができなかった場合アサート
    assert(device != nullptr);

    // 初期化成功をログに表示
    Log("Complete create D3D12Device!!!\n");

    /*===========================================================================================*/
    /*                                          本処理                                            */
    /*===========================================================================================*/

    /*--------------------------------- ウィンドウの処理 ----------------------------------*/
    
    // ウインドウの表示
    ShowWindow(hwnd, nCmdShow);

    // ウインドウの更新
    UpdateWindow(hwnd);


    /*---------------------------------- メインループ ------------------------------------*/

    MSG msg{};// メッセージを格納する変数

    while (msg.message != WM_QUIT) {

        // ウインドウにメッセージがある場合、優先して処理する
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        } else {
            // ゲームの処理
        }
    }

    return 0;
}

LRESULT CALLBACK WndProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        // 標準のメッセージ処理を行う
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}