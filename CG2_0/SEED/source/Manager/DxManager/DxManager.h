#pragma once

//
#include <windows.h>
#include <cstdint>
#include "DxFunc.h"
#include "matrixFunc.h"
#include "sphere.h"
#include "includes.h"
#include "PSOManager.h"
#include "PolygonManager.h"
#include "Camera.h"

//
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

class SEED;

class DxManager{
public:// 根幹をなす大枠の関数
    void Initialize(SEED* pSEED);
    void Finalize();

    void PreDraw();
    void DrawPolygonAll();
    void PostDraw();

private:// 内部の細かい関数

    void CreateDebugLayer();
    void CreateDevice();
    void CheckDebugLayer();

    void CreateCommanders();

    // SwapChain,ダブルバッファリングに関わる関数
    void CreateSwapChain();
    void GetSwapChainResources();
    void CreateAllDescriptorHeap();
    void CheckDescriptorSize();
    void CreateRTV();

    // Shaderのコンパイルに関わる関数
    void InitDxCompiler();
    void CompileShaders();

    // PSO
    void InitPSO();

    // preDraw,postDrawに関わる関数
    void TransitionResourceState(uint32_t state);
    void ClearViewSettings();

    // GPUとCPUの同期に関わる関数
    void CreateFence();
    void WaitForGPU();

public:// PolygonManagerに描画を頼む関数

    void DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Matrix4x4& worldMat, uint32_t color);

private:// 外部参照のためのポインタ

    SEED* pSEED_ = nullptr;
   
private:// マネージャ変数

    PSOManager* psoManager_ = nullptr;// PSOを作成するクラス
    PolygonManager* polygonManager_ = nullptr;

private:// オブジェクト

    Camera camera_;

public:
    
    // いろんなとこで実行結果を格納してくれる変数
    HRESULT hr;

    // CPU-GPUの同期のための変数
    ID3D12Fence* fence = nullptr;
    HANDLE fenceEvent;
    uint64_t fenceValue;

    // DirectX すべてのはじまり編
    ID3D12Debug1* debugController = nullptr;
    IDXGIFactory7* dxgiFactory = nullptr;
    IDXGIAdapter4* useAdapter = nullptr;// アダプタを格納する変数
    ID3D12Device* device = nullptr;// 生成したデバイスを格納する変数

    // commandList類
    ID3D12InfoQueue* infoQueue = nullptr;
    ID3D12CommandQueue* commandQueue = nullptr;
    ID3D12CommandAllocator* commandAllocator = nullptr;// コマンドアロケータを格納する変数
    ID3D12GraphicsCommandList* commandList = nullptr;// コマンドリストを格納する変数

    // SwapChain、ダブルバッファリングに必要な変数
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    IDXGISwapChain4* swapChain = nullptr;
    ID3D12Resource* swapChainResources[2] = { nullptr };
    uint32_t backBufferIndex;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    D3D12_RESOURCE_BARRIER barrier{};
    Vector4 clearColor;

    // ディスクリプタのサイズ
    uint32_t descriptorSizeSRV;
    uint32_t descriptorSizeRTV;
    uint32_t descriptorSizeDSV;

    // ディスクリプタヒープ類
    ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
    ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;
    ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;

    // dxcCompiler(HLSLをコンパイルするのに必要なもの)
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;
    IDxcIncludeHandler* includeHandler = nullptr;

    // VertexShader
    IDxcBlob* vertexShaderBlob = nullptr;
    // PixelShader
    IDxcBlob* pixelShaderBlob = nullptr;

    // PSO
    ID3D12PipelineState* graphicsPipelineState = nullptr;
    ID3D12RootSignature* rootSignature = nullptr;

    /*------------------------------ VertexResourceの作成 -------------------------------*/

    ID3D12Resource* vertexResource = nullptr;
    ID3D12Resource* indexResource = nullptr;
    ID3D12Resource* vertexResourceSprite = nullptr;
    ID3D12Resource* indexResourceSprite = nullptr;

    /*---------------------- TransformationMatrix用Resourceの作成 -----------------------*/

    ID3D12Resource* wvpResource = nullptr;
    /*-----スプライト用-----*/
    ID3D12Resource* wvpResourceSprite = nullptr;

    /*------------------------------ MaterialResourceの作成 -------------------------------*/

    ID3D12Resource* materialResource = nullptr;
    ID3D12Resource* materialResourceSprite = nullptr;
        
    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    ID3D12Resource* textureResource = nullptr;
    ID3D12Resource* intermediateResource = nullptr;
    ID3D12Resource* textureResource2 = nullptr;
    ID3D12Resource* intermediateResource2 = nullptr;

    /*------------------------- DepthStencilTextureResourceの作成 -------------------------*/

    ID3D12Resource* depthStencilResource = nullptr;

    /*----------------------------------LightingのResource---------------------------------*/

    ID3D12Resource* lightingResource = nullptr;
    DirectionalLight* directionalLight = nullptr;

    /*-------------------------scissor矩形とviewport-------------------------*/
    D3D12_VIEWPORT viewport{};
    D3D12_RECT scissorRect{};

    public:// アクセッサ

        Camera GetCamera()const{ return camera_; }
};