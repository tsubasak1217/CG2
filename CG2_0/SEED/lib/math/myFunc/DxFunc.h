#pragma once
#include "ModelData.h"
#include "Material.h"
#include "VertexData.h"
#include <string>
#include <format>
#include <cassert>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <DirectXTex.h>

#include "d3dx12.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

// ログ関数
void Log(const std::string& message);
void Log(const std::wstring& message);

// 文字列を変換する関数
std::wstring ConvertString(const std::string& str);
std::string ConvertString(const std::wstring& str);

// HLSLをコンパイルする関数
IDxcBlob* CompileShader(
    // CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    // CompilerProfile
    const wchar_t* profile,
    //初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler
);

// DescriptorHeap作成関数
ID3D12DescriptorHeap* CreateDescriptorHeap(
    ID3D12Device* device,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType, 
    UINT numDescriptors, 
    bool shaderVisible
);

void CreateDescriptorHeap(
    ID3D12Device* device,
    ID3D12DescriptorHeap* heap,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType, 
    UINT numDescriptors,
    bool shaderVisible
);

// テクスチャを読み込む関数
DirectX::ScratchImage LoadTextureImage(const std::string& filePath);

// Resource作成関数
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
ID3D12Resource* CreateTextureResource(ID3D12Device* device,const DirectX::TexMetadata& metadata);
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

// TextureResourceにデータを転送する関数
[[nodiscard]]
ID3D12Resource* UploadTextureData(
    ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
    ID3D12GraphicsCommandList* commandList);

// ディスクリプタのハンドルを取得する関数
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index);

// objファイルを読み込む関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

// Matrix4x4 を DirectX::XMMATRIX に変換する関数
DirectX::XMMATRIX ConvertToXMMATRIX(const Matrix4x4& matrix);