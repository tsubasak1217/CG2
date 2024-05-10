#include <windows.h>
#include <cstdint>
#include "myFunc.h"
#include "matrixFunc.h"
#include "sphere.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE,
    LPSTR, int nCmdShow
) {

    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    struct Transform {
        Vector3 scale_;
        Vector3 rotate_;
        Vector3 translate_;
    };

    struct VertexData {
        Vector4 position_;
        Vector2 texcoord_;
    };

    /*===========================================================================================*/
    /*                                   ウインドウの初期化                                         */
    /*===========================================================================================*/

     /*------------------------------ クライアント領域の決定 ------------------------------*/

    const int32_t kClientWidth = 1280;
    const int32_t kClientHeight = 720;
    RECT rect = { 0,0,kClientWidth,kClientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    /*------------------------------ ウインドウクラスの登録 ------------------------------*/

    WNDCLASS wc{};// 空のウインドウクラス
    TCHAR szAppName[] = L"test";// ウィンドウクラスの名前

    // ウインドウクラスの中身を記述
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szAppName;

    if(!RegisterClass(&wc)) { return 0; }

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

    if(!hwnd) { return 0; }

    /*===========================================================================================*/
    /*                                   DirextXの初期化                                          */
    /*===========================================================================================*/

         /*----------------------------- デバッグレイヤーの有効化 -----------------------------*/

#ifdef _DEBUG
    ID3D12Debug1* debugController = nullptr;
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    /*------------------------------- DXGIFactoryの生成 --------------------------------*/

    IDXGIFactory7* dxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    // 作成失敗していたらアサート
    assert(SUCCEEDED(hr));

    /*--------------------------- 使用するアダプタ(GPU)の決定 -----------------------------*/

    // アダプタを格納する変数の用意
    IDXGIAdapter4* useAdapter = nullptr;

    // 性能のいいGPUからループが先に来るようfor文を回す
    for(
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
        if(adapterDesc.Flags != DXGI_ADAPTER_FLAG3_SOFTWARE) {

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

    for(size_t i = 0; i < _countof(featureLevels); ++i) {

        // 先ほど決定したアダプタを使用してデバイスを生成
        hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

        // 生成に成功したらログを出力してループを終了
        if(SUCCEEDED(hr)) {
            Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
            break;
        }
    }

    // デバイス生成ができなかった場合アサート
    assert(device != nullptr);

    // 初期化成功をログに表示
    Log("Complete create D3D12Device!!!\n");

    /*--------------------- デバッグレイヤーでエラーが出た場合止める --------------------------*/

#ifdef _DEBUG
    ID3D12InfoQueue* infoQueue = nullptr;
    if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;

        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
#endif


    /*===========================================================================================*/
    /*                                 GPUに送る命令の作成とか                                      */
    /*===========================================================================================*/

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
    swapChainDesc.Width = kClientWidth;
    // 画面の高さ。 ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = kClientHeight;
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

    // RTVのディスクリプタヒープを作成
    ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(
        device,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,//レンダーターゲットビュー用に設定
        2,// ダブルバッファ用に2つ。 多くても別に構わない
        false
    );

    // SRVのディスクリプタヒープを作成
    ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(
        device,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,// SRV用に設定
        128,// ディスクリプタ数
        true
    );

    // ディスクリプタのサイズ
    const uint32_t descriptorSizeSRV = 
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const uint32_t descriptorSizeRTV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    const uint32_t descriptorSizeDSV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    descriptorSizeRTV;
    descriptorSizeDSV;

    /*------------------------- SwapChain から Resourceを引っ張ってくる --------------------------*/

    // リソースを格納する変数
    ID3D12Resource* swapChainResources[2] = { nullptr };

    // SwapChain から Resourceを引っ張ってくる
    // うまく取得できなければ起動できない
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));


    /*--------------------------------------- RTVの作成 ----------------------------------------*/

    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // ディスクリプタの先頭アドレスを取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // RTVを2つ作るのでディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};

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
    /*                                            HLSL                                           */
    /*===========================================================================================*/

    /*-------------------------- HLSL読み込みのためのコンパイラの初期化 ---------------------------*/

     // dxcCompiler
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;

    // インスタンスの作成
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
    IDxcIncludeHandler* includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));


    /*--------------------------------- RootSignatureの作成 ----------------------------------*/

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

    // RootSignatureに関する設定を記述していく
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    D3D12_ROOT_PARAMETER rootParameters[3] = {};

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CommandBufferViewを使用
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CommandBufferViewを使用
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    /*------- DescriptorRange,DescriptorTablの設定 -------*/
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;// 0から始まる
    descriptorRange[0].NumDescriptors = 1;// 数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offsetを自動計算

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;// Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);// Tableで利用する数

    descriptionRootSignature.pParameters = rootParameters; // ルートパラメーターへのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters); // パラメーターの配列数

    /*------------ Samplerの設定 ------------*/

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    /*--------------------------------------*/

    // シリアライズしてバイナリにする
    ID3DBlob* signatureBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    // バイナリを生成
    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    // 失敗した場合
    if(FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // RootSignatureの作成
    ID3D12RootSignature* rootSignature = nullptr;
    hr = device->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)
    );
    assert(SUCCEEDED(hr));

    /*--------------------------------- InputLayoutの設定 ----------------------------------*/

    // InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2]{};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    /*--------------------------------- BlendStateの設定 ----------------------------------*/

    D3D12_BLEND_DESC blendDesc{};
    // すべての色を書き込むよう設定
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    /*--------------------------------- RasterizerStateの設定 ----------------------------------*/

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;// 裏面を表示しない
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;// 三角形の中を塗りつぶす

    /*--------------------------------- shaderをコンパイルする -----------------------------------*/

    // VertexShader
    IDxcBlob* vertexShaderBlob = CompileShader(
        L"Object3d.VS.hlsl",
        L"vs_6_0",
        dxcUtils,
        dxcCompiler,
        includeHandler
    );
    assert(vertexShaderBlob != nullptr);

    // PixelShader
    IDxcBlob* pixelShaderBlob = CompileShader(
        L"Object3d.PS.hlsl",
        L"ps_6_0",
        dxcUtils,
        dxcCompiler,
        includeHandler
    );
    assert(pixelShaderBlob != nullptr);

    /*--------------------------- DepthStencilStateの作成 ---------------------------*/

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;// Depth機能有効化
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// 書き込みする
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;// 近いものを優先して描画

    /*--------------------------------- PSOの作成 -----------------------------------*/

    // RootSignature
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature;
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
    vertexShaderBlob->GetBufferSize() }; // VertexShader
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
    pixelShaderBlob->GetBufferSize() };// PixelShader
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;// DepsStencilState
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ (形状)のタイプ。 三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // どのように画面に色を打ち込むかの設定 (気にしなくて良い)
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 実際に生成
    ID3D12PipelineState* graphicsPipelineState = nullptr;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
    assert(SUCCEEDED(hr));

    /*------------------------------ VertexResourceの作成 -------------------------------*/

    ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6 * 16 * 16);
    ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

    /*---------------------- TransformationMatrix用Resourceの作成 -----------------------*/

    ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
    Matrix4x4* wvpData = nullptr;
    // wvpDataを読むように設定
    wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    // 単位行列を書き込む
    *wvpData = IdentityMat4();

    /*-----スプライト用-----*/
    ID3D12Resource* wvpResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
    Matrix4x4* wvpDataSprite = nullptr;
    // wvpDataを読むように設定
    wvpResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite));
    // 単位行列を書き込む
    *wvpDataSprite = IdentityMat4();

    Transform transformSprite = {
        {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}
    };

    /*------------------------------ MaterialResourceの作成 -------------------------------*/

    ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(VertexData));
    Vector4* materialData = nullptr;
    // materialDataを読むように設定
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    // materialDataに表示したい色を書き込む
    *materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    // 読み込み
    DirectX::ScratchImage mipImages = LoadTexture("resources/textures/uvChecker.png");
    // 作成
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
    // 転送
    ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

    // 読み込み
    DirectX::ScratchImage mipImages2 = LoadTexture("resources/textures/monsterBall.png");
    // 作成
    const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
    ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
    // 転送
    ID3D12Resource* intermediateResource2 = UploadTextureData(textureResource2, mipImages2, device, commandList);

    bool useMonsterBall = true;

    /*------------------------- DepthStencilTextureResourceの作成 -------------------------*/

    ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(
        device,
        kClientWidth,
        kClientHeight
    );

    /*-------------------------------- Texture用SRVの作成 ----------------------------------*/

    // metaDataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
    srvDesc2.Format = metadata2.format;
    srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

    // SRVを作成するDescriptor Heapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap,descriptorSizeSRV,1);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap,descriptorSizeSRV,1);

    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap,descriptorSizeSRV,2);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap,descriptorSizeSRV,2);

    // 先頭はImGuiが使っているのでその次を使う
    //textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    // SRVの生成
    device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
    device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

    /*------------------------------ VertexBufferViewの作成 -------------------------------*/

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
    // リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 6 * 16 * 16;
    vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
    // 1頂点あたりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);
    vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

    /*------------------------------ DepthStencilViewの作成 -------------------------------*/

    // DSV用のヒ－プはヒープタイプが違うので別途作る
    ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(
        device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false
    );

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(
        depthStencilResource,
        &dsvDesc,
        dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );


    /*----------------------------- 頂点リソースにデータを書き込む ----------------------------*/

    //頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    // 書き込むためのアドレスを取得
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    Sphere sphere(16);
    float kSubdivisionEvery = 1.0f / 16.0f;

    for(int latIdx = 0; latIdx < 16; ++latIdx){
        for(int lonIdx = 0; lonIdx < 16; ++lonIdx){

            vertexData[(latIdx * 16 * 6) + (lonIdx * 6)].position_ = sphere.vertexes_[latIdx][lonIdx];
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 1].position_ = sphere.vertexes_[latIdx][lonIdx + 1];
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 2].position_ = sphere.vertexes_[latIdx + 1][lonIdx + 1];
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 3].position_ = sphere.vertexes_[latIdx][lonIdx];
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 4].position_ = sphere.vertexes_[latIdx + 1][lonIdx + 1];
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 5].position_ = sphere.vertexes_[latIdx + 1][lonIdx];

            vertexData[(latIdx * 16 * 6) + (lonIdx * 6)].texcoord_ = { kSubdivisionEvery * lonIdx,kSubdivisionEvery * latIdx };
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 1].texcoord_ = { kSubdivisionEvery * (lonIdx + 1),kSubdivisionEvery * latIdx };
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 2].texcoord_ = { kSubdivisionEvery * (lonIdx + 1),kSubdivisionEvery * (latIdx + 1) };
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 3].texcoord_ = { kSubdivisionEvery * lonIdx,kSubdivisionEvery * latIdx };
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 4].texcoord_ = { kSubdivisionEvery * (lonIdx + 1),kSubdivisionEvery * (latIdx + 1) };
            vertexData[(latIdx * 16 * 6) + (lonIdx * 6) + 5].texcoord_ = { kSubdivisionEvery * lonIdx,kSubdivisionEvery * (latIdx + 1) };
        }
    }

    /*-----------------スプライト用----------------*/

     //頂点リソースにデータを書き込む
    VertexData* vertexDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
    // 左下
    vertexDataSprite[0].position_ = { 0.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[0].texcoord_ = { 0.0f,1.0f };
    // 左上
    vertexDataSprite[1].position_ = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[1].texcoord_ = { 0.0f,0.0f };
    // 右下
    vertexDataSprite[2].position_ = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[2].texcoord_ = { 1.0f,1.0f };

    // 左上
    vertexDataSprite[3].position_ = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[3].texcoord_ = { 0.0f,0.0f };
    // 右上
    vertexDataSprite[4].position_ = { 640.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[4].texcoord_ = { 1.0f,0.0f };
    // 右下
    vertexDataSprite[5].position_ = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[5].texcoord_ = { 1.0f,1.0f };

    /*--------------------------------- VewportとScissor ---------------------------------*/

    // ビューポート
    D3D12_VIEWPORT viewport{};
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = kClientWidth;
    viewport.Height = kClientHeight;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // シザー矩形
    D3D12_RECT scissorRect{};
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = kClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = kClientHeight;


    /*===========================================================================================*/
    /*                                          本処理                                            */
    /*===========================================================================================*/

    /*--------------------------------- ウィンドウの処理 ----------------------------------*/

    // ウインドウの表示
    ShowWindow(hwnd, nCmdShow);

    // ウインドウの更新
    UpdateWindow(hwnd);

    // 三角形を回す用の変数
    Transform transform(
        { 1.0f,1.0f,1.0f }, // scale
        { 0.0f,0.0f,0.0f }, // rotate
        { 0.0f,0.0f,0.0f }  // translate
    );
    // カメラの情報
    Transform cameraTransform(
        { 1.0f,1.0f,1.0f }, // scale
        { 0.0f,0.0f,0.0f }, // rotate
        { 0.0f,0.0f,-5.0f }  // translate
    );

    // 三角形のワールド行列の作成
    Matrix4x4 worldMatrix =
        AffineMatrix(transform.scale_, transform.rotate_, transform.translate_);
    // カメラのワールド行列の作成
    Matrix4x4 cameraMatrix =
        AffineMatrix(cameraTransform.scale_, cameraTransform.rotate_, cameraTransform.translate_);
    // ビュー行列の作成
    Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
    // 透視投影行列の作成
    Matrix4x4 projectionMatrix =
        PerspectiveMatrix(
            0.45f,
            AspectRatio(kClientWidth, kClientHeight),
            0.1f, 100.0f
        );
    // 掛け合わせてWVP行列の完成
    Matrix4x4 wvpMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

    /*--------スプライト用---------*/
    // 三角形のワールド行列の作成
    Matrix4x4 worldMatrixSprite =
        AffineMatrix(transformSprite.scale_, transformSprite.rotate_, transformSprite.translate_);
    // ビュー行列の作成
    Matrix4x4 viewMatrixSprite = InverseMatrix(cameraMatrix);
    // 透視投影行列の作成
    Matrix4x4 projectionMatrixSprite =
        OrthoMatrix(
            0.0f,
            kClientWidth,
            0.0f,
            kClientHeight,
            0.1f, 1000.0f
        );
    // 掛け合わせてWVP行列の完成
    Matrix4x4 wvpMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

    /*===========================================================================================*/
    /*                                        ImGuiの初期化                                       */
    /*===========================================================================================*/

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        device,
        swapChainDesc.BufferCount,
        rtvDesc.Format,
        srvDescriptorHeap,
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );

    /*---------------------------------- メインループ ------------------------------------*/

    MSG msg{};// メッセージを格納する変数

    while(msg.message != WM_QUIT) {

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ウインドウにメッセージがある場合、優先して処理する
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        ImGui::Begin("sprite");
        ImGui::DragFloat3("scale", &transformSprite.scale_.x, 0.025f);
        ImGui::DragFloat3("rotate", &transformSprite.rotate_.x, 3.14f * 0.01f);
        ImGui::DragFloat3("translate", &transformSprite.translate_.x, 1.0f);
        ImGui::End();

        // ゲームの処理

        /*---------------------- 三角形の回転 -----------------------*/

        // 変数の更新
        transform.rotate_.y += 0.03f;

        /*---------------------- 行列の再計算 -----------------------*/

        // 三角形のワールド行列の作成
        worldMatrix =
            AffineMatrix(transform.scale_, transform.rotate_, transform.translate_);
        // カメラのワールド行列の作成
        cameraMatrix =
            AffineMatrix(cameraTransform.scale_, cameraTransform.rotate_, cameraTransform.translate_);
        // ビュー行列の作成
        viewMatrix = InverseMatrix(cameraMatrix);
        // 透視投影行列の作成
        projectionMatrix =
            PerspectiveMatrix(
                0.45f,
                AspectRatio(kClientWidth, kClientHeight),
                0.1f, 100.0f
            );
        // 掛け合わせてWVP行列の完成
        wvpMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));


        /*--------Sprite-------*/

        // 三角形のワールド行列の作成
        worldMatrixSprite =
            AffineMatrix(transformSprite.scale_, transformSprite.rotate_, transformSprite.translate_);
        // ビュー行列の作成
        viewMatrixSprite = InverseMatrix(cameraMatrix);
        // 透視投影行列の作成
        projectionMatrixSprite =
            OrthoMatrix(
                0.0f,
                kClientWidth,
                0.0f,
                kClientHeight,
                0.1f, 1000.0f
            );
        // 掛け合わせてWVP行列の完成
        wvpMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));


        /*---------------------- 行列を書き込む -----------------------*/
        *wvpData = wvpMatrix;
        *wvpDataSprite = wvpMatrixSprite;

        /*------- これから書き込むバックバッファのインデックスを取得--------*/

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

        // 描画先のRTV,DSVを設定する
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

        // 指定した色で画面全体をクリアする
        float clearColor[]{ 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色。 RGBAの順
        commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
        // フレームの最初にもっとも遠くにクリアする
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        //------------------------------------------------------------
        ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
        commandList->SetDescriptorHeaps(1, descriptorHeaps);

        /*-------------------三角形描画コマンド積む---------------------*/

        commandList->RSSetViewports(1, &viewport); // Viewport
        commandList->RSSetScissorRects(1, &scissorRect); // Scissor
        // RootSignatureを設定。 PSOに設定しているけど別途設定が必要
        commandList->SetGraphicsRootSignature(rootSignature);
        commandList->SetPipelineState(graphicsPipelineState); // PSO

        ImGui::ShowDemoWindow();

        commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // VBV
        // 形状を設定。 PSOに設定しているものとはまた別。 同じものを設定すると考えておけば良い
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // マテリアルCBufferの設定
        commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
        // DescriptorTableの設定
        ImGui::Begin("monsterBall");
        ImGui::Checkbox("useMonsterBall", &useMonsterBall);
        ImGui::End();
        commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall? textureSrvHandleGPU2 : textureSrvHandleGPU);
        // wvp用のCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());


        ImGui::Render();


        // 描画! (DrawCall/ ドローコール)。 3頂点で1つのインスタンス。 インスタンスについては今後
        commandList->DrawInstanced(6 * 16 * 16, 1, 0, 0);

        commandList->SetGraphicsRootDescriptorTable(2,textureSrvHandleGPU);
        commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBV
        
        // wvp用のCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
        commandList->DrawInstanced(6, 1, 0, 0);

        // ImGuiの描画
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);


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
        if(fence->GetCompletedValue() < fenceValue) {

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

        //descriptorHeaps[0]->Release();


        ImGui::EndFrame();
    }


    /*===========================================================================================*/
    /*                                          後処理                                            */
    /*===========================================================================================*/

    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // オブジェクト類の解放
    CloseHandle(fenceEvent);
    //delete vertexData;
    depthStencilResource->Release();
    textureResource->Release();
    textureResource2->Release();
    intermediateResource->Release();
    intermediateResource2->Release();
    dsvDescriptorHeap->Release();
    materialResource->Release();
    wvpResource->Release();
    wvpResourceSprite->Release();
    vertexResource->Release();
    vertexResourceSprite->Release();
    graphicsPipelineState->Release();
    signatureBlob->Release();
    if(errorBlob) {
        errorBlob->Release();
    }
    rootSignature->Release();
    pixelShaderBlob->Release();
    vertexShaderBlob->Release();
    includeHandler->Release();
    dxcCompiler->Release();
    dxcUtils->Release();
    fence->Release();
    srvDescriptorHeap->Release();
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
    debugController->Release();
#endif
    CloseWindow(hwnd);


    // 解放漏れがないかチェック
    IDXGIDebug1* debug;
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }

    // COMの終了
    CoUninitialize();

    return 0;
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