#include "PolygonManager.h"
#include "DxManager.h"
#include "MyMath.h"
#include <assert.h>

/*------------------------ static, グローバル変数の初期化 ------------------------------*/

uint32_t PolygonManager::triangleIndexCount_ = 0;
D3D12_VERTEX_BUFFER_VIEW VBV_TriangleVertex_;

/*---------------------------------------------------------------------------------------------------------------*/
/*                                                                                                               */
/*                                             初期化処理・終了処理                                                 */
/*                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------*/

PolygonManager::PolygonManager(DxManager* pDxManager)
{
    pDxManager_ = pDxManager;
    triangles_.vertices.clear();
    triangles_.colorf.clear();
    triangleIndexCount_ = 0;
}

PolygonManager::~PolygonManager(){
    //Finalize();
}

void PolygonManager::InitResources()
{
    vertexResource_ =
        CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 3) * kMaxTriangleCount_);
    materialResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxTriangleCount_);
    wvpResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxTriangleCount_);
}

void PolygonManager::Finalize(){
    //vertexResource_->Release();
    //materialResource_->Release();
    //wvpResource_->Release();
}

void PolygonManager::Reset()
{
    triangles_.vertices.clear();
    triangles_.colorf.clear();
    triangles_.transform.clear();
    triangleIndexCount_ = 0;
}


/*---------------------------------------------------------------------------------------------------------------*/
/*                                                                                                               */
/*                                               描画に関わる関数                                                   */
/*                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------*/

/*---------------------------- 整数型のカラーコードをVector4に変換する関数 ------------------------------*/

Vector4 FloatColor(uint32_t color)
{
    float delta = 1.0f / 255.0f;

    Vector4 colorf = {
        float((color >> 24) & 0xff) * delta,
        float((color >> 16) & 0xff) * delta,
        float((color >> 8) & 0xff) * delta,
        float(color & 0xff) * delta
    };

    return colorf;
}


/*----------------------------------- 三角形の情報を追加する関数 ---------------------------------------*/

// DrawTriangleが呼び出されるごとに 三角形の情報を積み上げていく
void PolygonManager::AddTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Matrix4x4& worldMat, const Vector4& color,
    bool useTexture, bool view3D,uint32_t GH
){
    useTexture;
    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 transformed[3];

    transformed[0] = Multiply(TransformToVec3(v1), worldMat);
    transformed[1] = Multiply(TransformToVec3(v2), worldMat);
    transformed[2] = Multiply(TransformToVec3(v3), worldMat);

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(transformed[1].x, transformed[1].y, transformed[1].z) - Vector3(transformed[0].x, transformed[0].y, transformed[0].z),
            Vector3(transformed[2].x, transformed[2].y, transformed[2].z) - Vector3(transformed[1].x, transformed[1].y, transformed[1].z),
            view3D ? kWorld : kScreen
        ));

    Matrix4x4 wvp = Multiply(
        worldMat,
        view3D ?
        pDxManager_->GetCamera()->viewProjectionMat_ :
        pDxManager_->GetCamera()->viewProjectionMat2D_
    );

    // vertexResource
    triangles_.vertices.push_back(VertexData(v1, Vector2(0.5f, 0.0f), normalVec, color));
    triangles_.vertices.push_back(VertexData(v2, Vector2(1.0f, 1.0f), normalVec, color));
    triangles_.vertices.push_back(VertexData(v3, Vector2(0.0f, 1.0f), normalVec, color));

    // materialResource
    triangles_.colorf.push_back(Material());
    triangles_.colorf.back().color_ = color;
    triangles_.colorf.back().enableLighting_ = true;
    triangles_.colorf.back().uvTransform_ = IdentityMat4();

    // wvpResource
    triangles_.transform.push_back(TransformMatrix());
    triangles_.transform.back().world_ = worldMat;
    triangles_.transform.back().WVP_ = wvp;

    // GH
    triangles_.GH.push_back(GH);

    triangleIndexCount_++;
}

void PolygonManager::AddModel(Model model, uint32_t GH)
{
    model;
    GH;
    size_t modelSize = model.GetModelData().vertices.size();

    Matrix4x4 wvp = Multiply(
        model.GetWorldMat(),
        pDxManager_->GetCamera()->viewProjectionMat_
    );

    for(uint32_t i = 0; i < modelSize; i++){
        // vertexResource
        modelPolygons_.modelData.push_back(model.GetModelData());
    }

    // materialResource
    modelPolygons_.material.push_back(Material());
    modelPolygons_.material.back().color_ = {1.0f,1.0f,1.0f,1.0f};
    modelPolygons_.material.back().enableLighting_ = true;
    modelPolygons_.material.back().uvTransform_ = IdentityMat4();

    // wvpResource
    modelPolygons_.transform.push_back(TransformMatrix());
    modelPolygons_.transform.back().world_ = model.GetWorldMat();
    modelPolygons_.transform.back().WVP_ = wvp;

    // GraphHandle
    modelPolygons_.GH.push_back(GH);
}

void PolygonManager::SetTriangle()
{
    /*-------------------三角形描画コマンド積む---------------------*/

    // 三角形用のVertexBufferViewの設定
    //VBV_TriangleVertex_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    //VBV_TriangleVertex_.SizeInBytes = (sizeof(VertexData) * 3) * triangleIndexCount_;
    //VBV_TriangleVertex_.StrideInBytes = sizeof(VertexData);


    //// 書き込み
    //VertexData* vertexData;
    //Material* materialData;
    //TransformMatrix* transformData;

    //vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    //materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    //wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformData));

    //std::memcpy(vertexData, triangles_.vertices.data(), (sizeof(VertexData) * 3) * triangleIndexCount_);
    //std::memcpy(materialData, triangles_.colorf.data(), sizeof(Material) * triangleIndexCount_);
    //std::memcpy(transformData, triangles_.transform.data(), sizeof(TransformMatrix) * triangleIndexCount_);

    //D3D12_RANGE writeRange[3] = {
    //    {0,(sizeof(VertexData) * 3) * triangleIndexCount_},
    //    {0,sizeof(Material) * triangleIndexCount_},
    //    {0,sizeof(TransformMatrix) * triangleIndexCount_}
    //};

    //vertexResource_->Unmap(0, &writeRange[0]);
    //materialResource_->Unmap(0, &writeRange[1]);
    //wvpResource_->Unmap(0, &writeRange[2]);

    //// シザー矩形とviewport
    //pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport); // Viewport
    //pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect); // Scissor

    //// RootSignatureを設定。 PSOに設定しているけど別途設定が必要
    //pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->rootSignature.Get());
    //pDxManager_->commandList->SetPipelineState(pDxManager_->graphicsPipelineState.Get()); // PSO

    //pDxManager_->commandList->IASetVertexBuffers(0, 1, &VBV_TriangleVertex_); // VBV

    //// 形状を設定。 PSOに設定しているものとはまた別。 同じものを設定すると考えておけば良い
    //pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = 
    //    GetGPUDescriptorHandle(
    //        pDxManager_->srvDescriptorHeap.Get(),
    //        pDxManager_->descriptorSizeSRV,
    //        textureNum_
    //    );
    //pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

    //// Resourceを設定
    //pDxManager_->commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    //pDxManager_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    //pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());

    //// 描画! (DrawCall/ ドローコール)。 3頂点で1つのインスタンス。 インスタンスについては今後
    //pDxManager_->commandList->DrawInstanced(3 * triangleIndexCount_, 1, 0, 0);

    /*-------------------三角形描画コマンド積む---------------------*/

    VBV_TriangleVertex_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    VBV_TriangleVertex_.SizeInBytes = (sizeof(VertexData) * 3) * triangleIndexCount_;
    VBV_TriangleVertex_.StrideInBytes = sizeof(VertexData);

    /*-------------------共通の設定---------------------*/
    // シザー矩形とviewport
    pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport); // Viewport
    pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect); // Scissor

    // RootSignatureを設定。 PSOに設定しているけど別途設定が必要
    pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->rootSignature.Get());
    pDxManager_->commandList->SetPipelineState(pDxManager_->graphicsPipelineState.Get()); // PSO

    pDxManager_->commandList->IASetVertexBuffers(0, 1, &VBV_TriangleVertex_); // VBV

    // 形状を設定。 PSOに設定しているものとはまた別。 同じものを設定すると考えておけば良い
    pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Resourceを設定
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());

    /*-------------------書き込み---------------------*/

    VertexData* vertexData;
    Material* materialData;
    TransformMatrix* transformData;

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformData));
    
    std::memcpy(vertexData, triangles_.vertices.data(), (sizeof(VertexData) * 3) * triangleIndexCount_);
    std::memcpy(materialData, triangles_.colorf.data(), sizeof(Material) * triangleIndexCount_);
    std::memcpy(transformData, triangles_.transform.data(), sizeof(TransformMatrix) * triangleIndexCount_);

    D3D12_RANGE writeRange[3] = {
        {0,(sizeof(VertexData) * 3) * triangleIndexCount_},
        {0,sizeof(Material) * triangleIndexCount_},
        {0,sizeof(TransformMatrix) * triangleIndexCount_}
    };

    vertexResource_->Unmap(0, &writeRange[0]);
    materialResource_->Unmap(0, &writeRange[1]);
    wvpResource_->Unmap(0, &writeRange[2]);



    /*-------------------ひとつずつドロー---------------------*/
    for(uint32_t i = 0; i < triangleIndexCount_; i++){

        // テクスチャの設定
        D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(
            pDxManager_->srvDescriptorHeap.Get(),
            pDxManager_->descriptorSizeSRV,
            1 + triangles_.GH[i]// 0番目はimguiなので1から
        );

        pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

        // シェーディングレベルの初期値
        int32_t lowShadingPoint = 0;

        // カメラとの距離に応じてポイントを加算
        float distance = MyMath::Length(
            TransformToVec3(triangles_.vertices[i * 3].position_) -
            pDxManager_->GetCamera()->transform_.translate_
        );

        float depthRate = distance / pDxManager_->GetCamera()->zfar_;

        if(depthRate > 0.4f){
            lowShadingPoint += 4;
        } else if(depthRate > 0.2f){
            lowShadingPoint += 2;
        }

        // カメラに対する向きに応じてポイントを加算
        float dot = MyMath::Dot(triangles_.vertices[i * 3].normal_, pDxManager_->GetCamera()->normal_);
        if(dot >= 0.0f){
            lowShadingPoint += 4;
        } else if(dot <= -0.7f){

        } else if(dot <= std::cos(3.14f * 0.6)){
            lowShadingPoint += 2;
        } else if(dot <= std::cos(3.14f * 0.5f)){
            lowShadingPoint += 4;
        }

        // ポイントに応じてシェーディングレートを決定
        if(lowShadingPoint >= 4){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_4X4, nullptr);

        } else if(lowShadingPoint >= 2){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_2X2, nullptr);

        } else if(lowShadingPoint >= 0){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
        }

        // 描画! (DrawCall/ ドローコール)。 3頂点で1つのインスタンス。
        pDxManager_->commandList->DrawInstanced(
            3,
            1,
            3 * i,
            i
        );
    }

    // シェーディングレートをもとに戻す
    pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
}

void PolygonManager::SetModel()
{
}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawPolygonAll()
{
    // 三角形
    SetTriangle();

    // モデル
    SetModel();
}