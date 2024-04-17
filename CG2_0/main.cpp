#include <windows.h>
#include <cstdint>
#include "myFunc.h"
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

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
    TCHAR szAppName[] = L"test";// ウィンドウクラスの名前

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
        wc.hInstance,
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

    /*------------------------------- コマンドキューの生成 --------------------------------*/

    // 格納する変数
    ID3D12CommandQueue* commandQueue = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    // コマンドキューを生成する
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

    // コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));


    /*------------------------------- コマンドリストの生成 --------------------------------*/

    // コマンドアロケータを格納する変数
    ID3D12CommandAllocator* commandAllocator = nullptr;
    // コマンドアロケータを生成する
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    // コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));


    // コマンドリストを格納する変数
    ID3D12GraphicsCommandList* commandList = nullptr;
    // コマンドリストを生成する
    hr = device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)
    );
    // コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    /*------------------------------ スワップチェーンの作成 -------------------------------*/

    // スワップチェーンを生成する
    IDXGISwapChain4* swapChain = nullptr;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    // 画面の幅。 ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Width = kCliantWidth;
    // 画面の高さ。 ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = kCliantHeight;
    // 色の形式
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // マルチサンプルしない
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2; // ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄
    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue,
        hwnd,
        &swapChainDesc,
        nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1**>(&swapChain)
    );

    // 生成失敗したらアサート
    assert(SUCCEEDED(hr));


    /*------------------------------ ディスクリプタヒープの作成 -------------------------------*/

    // ディスクリプタヒープとディスクリプターを格納する変数
    ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};

    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー用に設定
    rtvDescriptorHeapDesc.NumDescriptors = 2; // ダブルバッファ用に2つ。 多くても別に構わない

    // ディスクリプタヒープを作成
    hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));

    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));


    /*------------------------- SwapChain から Resourceを引っ張ってくる --------------------------*/

    // リソースを格納する変数
    ID3D12Resource* swapChainResources[2] = { nullptr };

    // SwapChain から Resourceを引っ張ってくる
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));

    // うまく取得できなければ起動できない
    assert(SUCCEEDED(hr));
    assert(SUCCEEDED(hr));

    /*--------------------------------------- RTVの作成 ----------------------------------------*/

    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // ディスクリプタの先頭アドレスを取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // RTVを2つ作るのでディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

    // まず1つ目を作る。 1つ目は最初のところに作る。 作る場所をこちらで指定してあげる必要がある
    rtvHandles[0] = rtvStartHandle;
    // 2つ目のディスクリプタハンドルを得る (自力で)
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // RTVを2つ作る
    device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
    device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


    /*------------------------------ CPUとGPUの同期のための変数作成 --------------------------------*/

    //初期値でFenceを作る
    ID3D12Fence* fence = nullptr;
    uint64_t fenceValue = 0;
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    // FenceのSignalを待つためのイベントを作成する
    HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);


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

            // これから書き込むバックバッファのインデックスを取得
            UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

            /*--------------- TransitionBarrierを張る処理----------------*/

            D3D12_RESOURCE_BARRIER barrier{};
            // 今回のバリアはTransition
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            // None LTB
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            // バリアを張る対象のリソース。 現在のバックバッファに対して行う
            barrier.Transition.pResource = swapChainResources[backBufferIndex];
            // 遷移前 (現在) のResourceState
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            // 遷移後のResourceState
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            // TransitionBarrier
            commandList->ResourceBarrier(1, &barrier);

            /*----------------------------------------------------------*/

            // 描画先のRTVを設定する
            commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);

            // 指定した色で画面全体をクリアする
            float clearColor[]{ 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色。 RGBAの順
            commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

            /*----------------------------------------------------------*/

            // 画面に描く処理はすべて終わり、 画面に映すので、状態を遷移
            // 今回はRenderTargetからPresent にする
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

            // TransitionBarrierを張る
            commandList->ResourceBarrier(1, &barrier);

            /*----------------------------------------------------------*/

            // コマンドリストの内容を確定させる。 すべてのコマンドを積んでからCloseすること
            hr = commandList->Close();// コマンドリストの実行前に閉じる
            assert(SUCCEEDED(hr));

            // GPUにコマンドリストの実行を行わせる
            ID3D12CommandList* commandLists[] = { commandList };
            commandQueue->ExecuteCommandLists(1, commandLists);

            // GPUとOSに画面の交換を行うよう通知する
            swapChain->Present(1, 0);

            /*---------------------- CPUとGPUの同期 ----------------------*/

            // Fenceの値を更新
            fenceValue++;
            // GPUがここまでたどり着いたときに、 Fenceの値を指定した値に代入するようにSignalを送る
            commandQueue->Signal(fence, fenceValue);

            // Fenceの値が指定したSignal値にたどり着いているか確認する
            // GetCompletedValueの初期値はFence作成時に渡した初期値
            if (fence->GetCompletedValue() < fenceValue) {

                // 指定したSignalにたどりついていないので、GPUがたどり着くまで待つようにイベントを設定する
                fence->SetEventOnCompletion(fenceValue, fenceEvent);
                // CPUはイベント待つ
                WaitForSingleObject(fenceEvent, INFINITE);

            }

            /*-----------------------------------------------------------*/

            // リセットして次のフレーム用のコマンドリストを準備
            hr = commandAllocator->Reset();
            assert(SUCCEEDED(hr));
            hr = commandList->Reset(commandAllocator, nullptr);
            assert(SUCCEEDED(hr));


        }
    }

    // オブジェクト類の解放
    CloseHandle(fenceEvent);
    fence->Release();
    rtvDescriptorHeap->Release();
    swapChainResources[0]->Release();
    swapChainResources[1]->Release();
    swapChain->Release();
    commandList->Release();
    commandAllocator->Release();
    commandQueue->Release();
    device->Release();
    useAdapter->Release();
    dxgiFactory->Release();
#ifdef _DEBUG
    //debugController->Release();
#endif
    CloseWindow(hwnd);


    // 解放漏れがないかチェック
    IDXGIDebug1* debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
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