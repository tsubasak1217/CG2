#include "DxManager.h"
#include "SEED.h"
#include "Environment.h"
#include "MyMath.h"

void DxManager::Initialize(SEED* pSEED)
{

    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // ポインタ代入
    pSEED_ = pSEED;
    // PSOManagerの作成
    psoManager_ = new PSOManager(this);
    // polygonManagerの作成
    polygonManager_ = new PolygonManager(this);
    //
    camera_ = new Camera();

    /*===========================================================================================*/
    /*                                   DirextXの初期化                                          */
    /*===========================================================================================*/

    /*------------ デバッグレイヤーの有効化 ---------------*/

#ifdef _DEBUG
    CreateDebugLayer();
#endif

    /* --------------  Deviceの生成 -------------------- */

    CreateDevice();

    /*---------- デバッグレイヤーでエラーが出た場合止める ----------*/

#ifdef _DEBUG
    CheckDebugLayer();
#endif


    /*===========================================================================================*/
    /*                                 GPUに送る命令の作成とか                                      */
    /*===========================================================================================*/

    /*----- コマンドに関わるものの生成 -------*/

    // GPUに命令を投げるやつとか命令の入れ物を作る
    CreateCommanders();

    /*===========================================================================================*/
    /*                               ダブルバッファリングにしよう                                    */
    /*===========================================================================================*/

    /*--------------------- スワップチェーンの作成 --------------------------*/

    // SwapChain ~ 画面を複数用意し、表示されていない画面で描画を同時進行で行う
    CreateSwapChain();

    /*--------------- SwapChain から Resourceを引っ張ってくる ---------------*/

    // 【 Resource 】 ~ GPUのメモリ上にあるデータのうち、描画や表示に関わるものの総称。
    // 画面そのものもResourceなので、ここで取得
    GetSwapChainResources();

    /*---------------------- ディスクリプタヒープの作成 ----------------------*/

    /*
       【 View 】 ~ Resourceに対してどのような処理を行うのかをまとめたもの。Resourceを操作するにはViewが必要。
       【 Descriptor 】~ Viewを格納する場所。

        Viewの作成    : CPU
        Viewの置き場所 : CPU (descriptor) 
        Viewの実行    : GPU (CPU上のdescriptorを参照できる)
    */

    CreateAllDescriptorHeap();

    // ディスクリプタのサイズを確認
    CheckDescriptorSize();

    /*----------------------------- RTVの作成 -----------------------------*/

    /*
        ここでは "描く" という処理を行いたいため、
        描画担当のView、"RTV"(RenderTargetVier) を作成
    */
    CreateRTV();

    /*------------------- CPUとGPUの同期のための変数作成 ---------------------*/

    /*
        GPUからfenceに対して、fenceValueの値が書き込まれるまでResetを待つようにする。
        そのために必要な変数を用意しておく
    */
    CreateFence();

    /*===========================================================================================*/
    /*                                ポリゴンを描画していくゾーン                                    */
    /*===========================================================================================*/


    /*---------------------- HLSLコンパイル --------------------------*/

    /*
        ポリゴンを描画するにはShaderの情報が必要。
        Shaderの中身はGPUで処理をするため、GPUが読める形式に
        変換しないといけない。
    */

    // Shaderコンパイルのためのコンパイラの初期化
    InitDxCompiler();

    // shaderをコンパイルする
    CompileShaders();

    /*------------------------PSOの生成-----------------------*/

    /*
                            ~ PSOとは ~
          長ったらしく工程数の多いレンダリングパイプラインの設定を
          個別で行うのではなくまとめて1つの変数に集約して行うことで
            アクセスの高速化、メンテナンス性などを向上させている。
        入力された情報を加工するのに関わるものがここに集約されている。工場。
    */

    // PSOManagerクラスに丸投げ
    InitPSO();

    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    // 読み込み
    DirectX::ScratchImage mipImages = LoadTexture("resources/textures/uvChecker.png");
    // 作成
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    textureResource = CreateTextureResource(device, metadata);
    // 転送
    intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

    // 読み込み
    DirectX::ScratchImage mipImages2 = LoadTexture("resources/textures/monsterBall.png");
    // 作成
    const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
    textureResource2 = CreateTextureResource(device, metadata2);
    // 転送
    intermediateResource2 = UploadTextureData(textureResource2, mipImages2, device, commandList);

    /*------------------------- DepthStencilTextureResourceの作成 -------------------------*/

    depthStencilResource = CreateDepthStencilTextureResource(
        device,
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_
    );

    /*----------------------------------LightingのResource---------------------------------*/

    lightingResource = CreateBufferResource(device, sizeof(DirectionalLight));
    lightingResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight));

    directionalLight->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight->direction_ = { 0.0f,0.0f,1.0f };
    directionalLight->intensity = 1.0f;

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


    // SRVを作成するDescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);

    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
    
    // SRVの生成
    device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
    device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);


    /*------------------------------ DepthStencilViewの作成 -------------------------------*/

    // DSV用のヒ－プはヒープタイプが違うので別途作る
    dsvDescriptorHeap = CreateDescriptorHeap(
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

    /*----------------------------------UVTransformの設定----------------------------------*/

    //materialData->uvTransform_ = IdentityMat4();
    //materialDataSprite->uvTransform_ = IdentityMat4();

    //Transform uvTransformSprite;
    //uvTransformSprite.scale_ = { 1.0f,1.0f,1.0f };
    //uvTransformSprite.rotate_ = { 0.0f,0.0f,0.0f };
    //uvTransformSprite.translate_ = { 0.0f,0.0f,0.0f };

    //// uvTransformを行う行列
    //uvTransformSprite.world_ = AffineMatrix(
    //    uvTransformSprite.scale_,
    //    uvTransformSprite.rotate_,
    //    uvTransformSprite.translate_
    //);

    //// 行列を代入
    //materialDataSprite->uvTransform_ = uvTransformSprite.world_;

    /*--------------------------------- VewportとScissor ---------------------------------*/

    // ビューポート
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = (float)pSEED_->kClientWidth_;
    viewport.Height = (float)pSEED_->kClientHeight_;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    
    // シザー矩形
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = pSEED_->kClientWidth_;
    scissorRect.bottom = pSEED_->kClientHeight_;


    // ------------------------------------------------------------------------------------

    // カメラの情報
    camera_->transform_.scale_ = { 1.0f,1.0f,1.0f }; // scale
    camera_->transform_.rotate_ = { 0.0f,0.0f,0.0f }; // rotate
    camera_->transform_.translate_ = { 0.0f,0.0f,-10.0f }; // translate
    camera_->projectionMode_ = ORTHO;
    camera_->clipRange_ = kWindowSize;
    camera_->znear_ = 0.1f;
    camera_->zfar_ = 100.0f;
    camera_->Update();

    // 情報がそろったのでpolygonManagerの初期化
    polygonManager_->InitResources();
}


void DxManager::PreDraw()
{
    /*------- これから書き込むバックバッファのインデックスを取得--------*/

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    /*--------------- TransitionBarrierを張る処理----------------*/

    // RTVを実行するために表示されていない後ろの画面(バックバッファ)の状態を描画状態に遷移させる
    TransitionResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);

    /*----------画面、深度情報のクリア、DSVとRTVの結び直し-----------*/

    ClearViewSettings();

    /*----------------- SRVのヒープの再セット --------------------*/
    /*
        imGuiがフレーム単位でHeapの中身を操作するため
        SRVのHeapは毎フレームセットし直す
    */
    commandList->SetDescriptorHeaps(1, &srvDescriptorHeap);

    //ImGui::Begin("Camera");
    //ImGui::DragFloat3("translate", &camera_->transform_.translate_.x, 0.05f);
    //ImGui::DragFloat3("rotate", &camera_->transform_.rotate_.x, 3.14f * 0.005f);
    //ImGui::End();
}

void DxManager::DrawPolygonAll()
{
    polygonManager_->DrawPolygonAll();
}

void DxManager::PostDraw()
{
    // 画面に描く処理はすべて終わり、 画面に映すので、状態を遷移
    // 今回はRenderTargetからPresent にする
    TransitionResourceState(D3D12_RESOURCE_STATE_PRESENT);

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

    WaitForGPU();// CPUはGPUの処理が追いつくまでここで待機！

    /*----------------- PolygonManagerのリセット -----------------*/

    polygonManager_->Reset();

    /*------------------- commandListのリセット ------------------*/
    // リセットして次のフレーム用のコマンドリストを準備
    hr = commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList->Reset(commandAllocator, nullptr);
    assert(SUCCEEDED(hr));
}

void DxManager::CreateDebugLayer()
{
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
}

void DxManager::CreateDevice()
{
    /*------------------------------- DXGIFactoryの生成 --------------------------------*/

    hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    // 作成失敗していたらアサート
    assert(SUCCEEDED(hr));

    /*--------------------------- 使用するアダプタ(GPU)の決定 -----------------------------*/

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
}

void DxManager::CheckDebugLayer()
{
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
}

void DxManager::CreateCommanders()
{

    /*------------------------------- コマンドキューの生成 --------------------------------*/

    // 格納する変数
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    // コマンドキューを生成する
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

    // コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));


    /*------------------------------- コマンドリストの生成 --------------------------------*/

    // コマンドアロケータを生成する
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    // コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    // コマンドリストを生成する
    hr = device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)
    );
    // コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
}

void DxManager::CreateSwapChain()
{
    // 画面の幅。 ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Width = pSEED_->kClientWidth_;
    // 画面の高さ。 ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = pSEED_->kClientHeight_;
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
        pSEED_->hwnd,
        &swapChainDesc,
        nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1**>(&swapChain)
    );

    // 生成失敗したらアサート
    assert(SUCCEEDED(hr));
}

void DxManager::GetSwapChainResources()
{
    // SwapChain から Resourceを引っ張ってくる
// うまく取得できなければ起動できない
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

}

void DxManager::CreateAllDescriptorHeap()
{
    // RTVのディスクリプタヒープを作成
    rtvDescriptorHeap = CreateDescriptorHeap(
        device,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,//レンダーターゲットビュー用に設定
        2,// ダブルバッファ用に2つ。 多くても別に構わない
        false
    );

    // SRVのディスクリプタヒープを作成
    srvDescriptorHeap = CreateDescriptorHeap(
        device,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,// SRV用に設定
        128,// ディスクリプタ数
        true
    );
}

void DxManager::CheckDescriptorSize()
{
    // ディスクリプタのサイズ
    descriptorSizeSRV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeRTV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void DxManager::CreateRTV()
{
    // RTVの設定
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // ダブルバッファなのでRTVを2つ作る。
    // ディスクリプタヒープの先頭から作成するので先頭アドレスを取得し、順番に入れていく
    rtvHandles[0] = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandles[1].ptr = rtvHandles[0].ptr + descriptorSizeRTV;

    // 取得したアドレスにRTVを2つ作る
    device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
    device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
}

void DxManager::CreateFence()
{
    //初期値でFenceを作る
    fenceValue = 0;
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    // FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
}

void DxManager::InitDxCompiler()
{
    // インスタンスの作成
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));
}

void DxManager::CompileShaders()
{
    // VertexShader
    vertexShaderBlob = CompileShader(
        L"resources/shaders/Object3d.VS.hlsl",
        L"vs_6_0",
        dxcUtils,
        dxcCompiler,
        includeHandler
    );
    assert(vertexShaderBlob != nullptr);

    // PixelShader
    pixelShaderBlob = CompileShader(
        L"resources/shaders/Object3d.PS.hlsl",
        L"ps_6_0",
        dxcUtils,
        dxcCompiler,
        includeHandler
    );
    assert(pixelShaderBlob != nullptr);
}

void DxManager::InitPSO()
{
    psoManager_->Create();
}


void DxManager::TransitionResourceState(uint32_t state)
{
    switch(state)
    {
    case D3D12_RESOURCE_STATE_RENDER_TARGET:// 描く状態にする----------------

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

        break;

    case D3D12_RESOURCE_STATE_PRESENT:// 写す状態にする---------------------

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        break;

    default:
        break;
    }
    
    // リソースのstateを変更
    commandList->ResourceBarrier(1, &barrier);
}

void DxManager::ClearViewSettings()
{
    // 指定した色で画面全体をクリアする
    clearColor = MyMath::FloatColor(windowBackColor);
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], &clearColor.x, 0, nullptr);

    // 描画先のRTV,DSVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // DSVと現在のRTVを結びつける
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

    // フレームの最初にもっとも遠くにクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DxManager::WaitForGPU()
{
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
}

void DxManager::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3, 
    const Matrix4x4& worldMat, const Vector4& color,
    bool useTexture, bool view3D
){
    polygonManager_->AddTriangle(v1, v2, v3, worldMat,color,useTexture,view3D);
}

void DxManager::Finalize()
{
    /*===========================================================================================*/
    /*                                          後処理                                            */
    /*===========================================================================================*/

    // オブジェクト類の解放
    CloseHandle(fenceEvent);
    //delete vertexData;
    depthStencilResource->Release();
    textureResource->Release();
    textureResource2->Release();
    intermediateResource->Release();
    intermediateResource2->Release();
    dsvDescriptorHeap->Release();
    lightingResource->Release();
    //materialResource->Release();
    //materialResourceSprite->Release();
    //wvpResource->Release();
    //wvpResourceSprite->Release();
    //vertexResource->Release();
    //vertexResourceSprite->Release();
    //indexResourceSprite->Release();
    //indexResource->Release();
    graphicsPipelineState->Release();
    //signatureBlob->Release();
    //if(errorBlob) {
    //    errorBlob->Release();
    //}
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

    delete psoManager_;
    psoManager_ = nullptr;
    delete polygonManager_;
    polygonManager_ = nullptr;
    delete camera_;
    camera_ = nullptr;

#ifdef _DEBUG
    debugController->Release();
#endif
    CloseWindow(pSEED_->hwnd);


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
}