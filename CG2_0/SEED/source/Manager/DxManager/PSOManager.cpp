#include "PSOManager.h"
#include "DxManager.h"

PSOManager::PSOManager(DxManager* pDxManager)
{
    pDxManager_ = pDxManager;
}

PSOManager::~PSOManager()
{
}

void PSOManager::Create()
{
    HRESULT hr;
    ID3DBlob* signatureBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    /*-------------------- RootSignatureの作成 ----------------------*/

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

    // RootSignatureに関する設定を記述していく
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    /*--------------- ルートパラメーターの設定 ----------------*/
    D3D12_ROOT_PARAMETER rootParameters[4] = {};

    //=============================================================================================//
    /*---------------------- material ------------------------*/
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;// SRVを使用
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号t0とバインド

    /*---------------------- transform ------------------------*/
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;// SRVを使用
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号t0とバインド

    /*------------------------ texture ------------------------*/
    //DescriptorRange,DescriptorTableの設定
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 1;// t1から始まる
    descriptorRange[0].NumDescriptors = 1;// 数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offsetを自動計算

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;// Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);// Tableで利用する数

    /*----------------------Lighting---------------------------*/
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// DescriptorTableを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[3].Descriptor.ShaderRegister = 0;// レジスタ番号b0を使う

    //=============================================================================================//

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
    staticSamplers[0].ShaderRegister = 0; // レジスタ番号s0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    /*--------------------------------------*/

    // バイナリを生成
    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    // 失敗した場合
    if(FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // RootSignatureの作成
    hr = pDxManager_->device->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(pDxManager_->rootSignature.GetAddressOf())
    );
    assert(SUCCEEDED(hr));

    /*--------------------------------- InputLayoutの設定 ----------------------------------*/

    /*
                     ~ InputLayoutとは ~
    CPUからVertexShaderに渡すデータがどのようなものなのか指定するもの
           VertexShaderInputにあるメンバ変数の数だけ設定する
    */
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[4]{};

    // 頂点座標
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // テクスチャ座標
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // 法線ベクトル
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // 色
    inputElementDescs[3].SemanticName = "INDEX";
    inputElementDescs[3].SemanticIndex = 0;
    inputElementDescs[3].Format = DXGI_FORMAT_R32_UINT;
    inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


    // 上で作成したデータ群をひとつの変数に入れる
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

    /*--------------------------- DepthStencilStateの作成 ---------------------------*/

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;// Depth機能有効化
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// 書き込みする
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;// 近いものを優先して描画

    /*--------------------------------- PSOの作成 -----------------------------------*/

    // RootSignature
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = pDxManager_->rootSignature.Get();
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
    graphicsPipelineStateDesc.VS = { pDxManager_->vertexShaderBlob->GetBufferPointer(),
    pDxManager_->vertexShaderBlob->GetBufferSize() }; // VertexShader
    graphicsPipelineStateDesc.PS = { pDxManager_->pixelShaderBlob->GetBufferPointer(),
    pDxManager_->pixelShaderBlob->GetBufferSize() };// PixelShader
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
    hr = pDxManager_->device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(pDxManager_->graphicsPipelineState.GetAddressOf()));


    // 解放
    signatureBlob->Release();;
    if(errorBlob){
        errorBlob->Release();
    }

    assert(SUCCEEDED(hr));
}
