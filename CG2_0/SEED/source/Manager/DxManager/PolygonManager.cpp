#include "PolygonManager.h"
#include "DxManager.h"
#include "MyMath.h"
#include <assert.h>

/*------------------------ static, グローバル変数の初期化 ------------------------------*/

uint32_t PolygonManager::triangleIndexCount_ = 0;
uint32_t PolygonManager::modelIndexCount_ = 0;
D3D12_VERTEX_BUFFER_VIEW VBV_TriangleVertex_;
D3D12_VERTEX_BUFFER_VIEW VBV_ModelVertex_;

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
    // triangle
    vertexResource_ =
        CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 3) * kMaxTriangleCount_);
    materialResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxTriangleCount_);
    wvpResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxTriangleCount_);

    // model
    vertexResource_model_ =
        CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * kMaxModelVertexCount) * kMaxModelCount_);
    materialResource_model_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxModelCount_);
    wvpResource_model_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxModelCount_);
}

void PolygonManager::Finalize(){}

void PolygonManager::Reset()
{
    triangles_.vertices.clear();
    triangles_.colorf.clear();
    triangles_.transform.clear();
    triangles_.GH.clear();
    triangles_.isView3D.clear();
    triangleIndexCount_ = 0;


    modelPolygons_.modelData.clear();
    modelPolygons_.material.clear();
    modelPolygons_.transform.clear();
    modelPolygons_.GH.clear();
    modelIndexCount_ = 0;
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
    bool enableLighting, const Matrix4x4& uvTransform, bool view3D, uint32_t GH
){
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
    triangles_.vertices.push_back(VertexData(v1, Vector2(0.5f, 0.0f), normalVec, triangleIndexCount_));
    triangles_.vertices.push_back(VertexData(v2, Vector2(1.0f, 1.0f), normalVec, triangleIndexCount_));
    triangles_.vertices.push_back(VertexData(v3, Vector2(0.0f, 1.0f), normalVec, triangleIndexCount_));

    // materialResource
    triangles_.colorf.push_back(Material());
    triangles_.colorf.back().color_ = color;
    triangles_.colorf.back().enableLighting_ = enableLighting;
    triangles_.colorf.back().uvTransform_ = uvTransform;

    // wvpResource
    triangles_.transform.push_back(TransformMatrix());
    triangles_.transform.back().world_ = worldMat;
    triangles_.transform.back().WVP_ = wvp;

    // GH
    triangles_.GH.push_back(GH);
    // 2D or 3D
    triangles_.isView3D.push_back(view3D);

    triangleIndexCount_++;
}

void PolygonManager::AddModel(Model model)
{

    Matrix4x4 wvp = Multiply(
        model.GetWorldMat(),
        pDxManager_->GetCamera()->viewProjectionMat_
    );

    // vertexResource
    modelPolygons_.modelData.push_back(model.GetModelData());


    // materialResource
    modelPolygons_.material.push_back(Material());
    modelPolygons_.material.back().color_ = { 1.0f,1.0f,1.0f,1.0f };
    modelPolygons_.material.back().enableLighting_ = model.GetEnableLighting();
    modelPolygons_.material.back().uvTransform_ = IdentityMat4();

    // wvpResource
    modelPolygons_.transform.push_back(TransformMatrix());
    modelPolygons_.transform.back().world_ = model.GetWorldMat();
    modelPolygons_.transform.back().WVP_ = wvp;

    // GraphHandle
    modelPolygons_.GH.push_back(model.GetTextureGH());

    modelIndexCount_++;
}

void PolygonManager::SetTriangle()
{
    /*===========================================================================================*/
    /*                               すべての三角形に共通の設定                                      */
    /*===========================================================================================*/

    // VertexBufferViewの作成
    VBV_TriangleVertex_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    VBV_TriangleVertex_.SizeInBytes = (sizeof(VertexData) * 3) * triangleIndexCount_;
    VBV_TriangleVertex_.StrideInBytes = sizeof(VertexData);

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
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(0, materialResource_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(1, wvpResource_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());

    /*===========================================================================================*/
    /*                                  三角形の情報の書き込み                                      */
    /*===========================================================================================*/

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

    /*===========================================================================================*/
    /*                                     三角形を個別に描画                                       */
    /*===========================================================================================*/
    for(uint32_t i = 0; i < triangleIndexCount_; i++){

        /*///////////////////////////////////////////////*/

        /*                 テクスチャの設定                 */

        /*///////////////////////////////////////////////*/


        // グラフハンドルに応じたテクスチャハンドルを得る
        D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(
            pDxManager_->srvDescriptorHeap.Get(),
            pDxManager_->descriptorSizeSRV,
            1 + triangles_.GH[i]// 0番目はimguiなので1から
        );

        // テクスチャをセット
        pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


        /*///////////////////////////////////////////////*/

        /* 解像度(ピクセルシェーダーで処理するピクセル単位)の設定 */

        /*///////////////////////////////////////////////*/


        // シェーディングレベルの初期値(この数値に応じて解像度を決定)
        int32_t lowShadingPoint = 0;

        /*-------- カメラとの距離に応じてポイントを加算 --------*/

        if(triangles_.isView3D[i]){// 2D三角形のときは処理しない

            // カメラとの距離
            float distance = MyMath::Length(
                TransformToVec3(triangles_.vertices[i * 3].position_) -
                pDxManager_->GetCamera()->transform_.translate_
            );

            // 最深(zfar)と比較した割合を算出
            float depthRate = distance / pDxManager_->GetCamera()->zfar_;

            // 割合に応じてポイントを決定
            if(depthRate > 0.4f){
                lowShadingPoint += 4;
            } else if(depthRate > 0.2f){
                lowShadingPoint += 2;
            }
        }

        /*-------- カメラに対する向きに応じてポイントを加算 --------*/

        // 法線同士の内積を取る
        float dot = MyMath::Dot(triangles_.vertices[i * 3].normal_, pDxManager_->GetCamera()->normal_);

        // 正規かベクトル同士の内積 = cos(θ) なのでその値から角度が分かる
        if(dot >= 0.0f){
            lowShadingPoint += 4;
        } else if(dot <= -0.7f){

        } else if(dot <= std::cos(3.14f * 0.6)){
            lowShadingPoint += 2;
        } else if(dot <= std::cos(3.14f * 0.5f)){
            lowShadingPoint += 4;
        }

        /*------------ ポイントに応じて解像度を決定 -----------*/

        if(lowShadingPoint >= 4){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_4X4, nullptr);

        } else if(lowShadingPoint >= 2){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_2X2, nullptr);

        } else if(lowShadingPoint >= 0){
            pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
        }


        /*///////////////////////////////////////////////*/

        /*      　   描画! 3頂点で1つのインスタンス           */

        /*///////////////////////////////////////////////*/

        pDxManager_->commandList->DrawInstanced(
            3,
            1,
            3 * i,// VB上のどの頂点から描画するか
            0
        );
    }

    // シェーディングレートをもとに戻す
    pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
}

void PolygonManager::SetModel()
{
    /*===========================================================================================*/
    /*                               すべての三角形に共通の設定                                      */
    /*===========================================================================================*/

    // シザー矩形とviewport
    pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport); // Viewport
    pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect); // Scissor

    // RootSignatureを設定。 PSOに設定しているけど別途設定が必要
    pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->rootSignature.Get());
    pDxManager_->commandList->SetPipelineState(pDxManager_->graphicsPipelineState.Get()); // PSO

    // 形状を設定。 PSOに設定しているものとはまた別。 同じものを設定すると考えておけば良い
    pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Resourceを設定
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(0, materialResource_model_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(1, wvpResource_model_->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());

    /*===========================================================================================*/
    /*                                  三角形の情報の書き込み                                      */
    /*===========================================================================================*/

    VertexData* vertexData;
    Material* materialData;
    TransformMatrix* transformData;

    vertexResource_model_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    materialResource_model_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    wvpResource_model_->Map(0, nullptr, reinterpret_cast<void**>(&transformData));

    int vertexCountAll = 0;
    for(int i = 0; i < modelPolygons_.modelData.size(); i++){
        std::memcpy(
            vertexData, 
            modelPolygons_.modelData[i].vertices.data(),
            sizeof(VertexData) * (int)modelPolygons_.modelData[i].vertices.size()
        );
        vertexCountAll += (int)modelPolygons_.modelData[i].vertices.size();
    }

    std::memcpy(materialData, modelPolygons_.material.data(), sizeof(Material) * modelIndexCount_);
    std::memcpy(transformData, modelPolygons_.transform.data(), sizeof(TransformMatrix) * modelIndexCount_);

    D3D12_RANGE writeRange[3] = {
        {0,sizeof(VertexData) * vertexCountAll},
        {0,sizeof(Material) * modelIndexCount_},
        {0,sizeof(TransformMatrix) * modelIndexCount_}
    };

    vertexResource_model_->Unmap(0, &writeRange[0]);
    materialResource_model_->Unmap(0, &writeRange[1]);
    wvpResource_model_->Unmap(0, &writeRange[2]);

    /*===========================================================================================*/
    /*                                     三角形を個別に描画                                       */
    /*===========================================================================================*/

    for(uint32_t i = 0; i < modelIndexCount_; i++){

        /*///////////////////////////////////////////////*/

        /*                   VBVの設定                    */

        /*///////////////////////////////////////////////*/

        // VertexBufferViewの作成
        VBV_ModelVertex_.BufferLocation = vertexResource_model_->GetGPUVirtualAddress();
        VBV_ModelVertex_.SizeInBytes = sizeof(VertexData) * (int)modelPolygons_.modelData[i].vertices.size();
        VBV_ModelVertex_.StrideInBytes = sizeof(VertexData);

        pDxManager_->commandList->IASetVertexBuffers(0, 1, &VBV_ModelVertex_); // VBVのセット


        /*///////////////////////////////////////////////*/

        /*                 テクスチャの設定                 */

        /*///////////////////////////////////////////////*/


        // グラフハンドルに応じたテクスチャハンドルを得る
        D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(
            pDxManager_->srvDescriptorHeap.Get(),
            pDxManager_->descriptorSizeSRV,
            1 + modelPolygons_.GH[i]// 0番目はimguiなので1から
        );

        // テクスチャをセット
        pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


        /*///////////////////////////////////////////////*/

        /*      　              描画                      */

        /*///////////////////////////////////////////////*/

        pDxManager_->commandList->DrawInstanced(
            (int)modelPolygons_.modelData[i].vertices.size(),
            1,
            0,
            0
        );

    }

    // シェーディングレートをもとに戻す
    pDxManager_->commandList->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawPolygonAll()
{
    // 三角形
    SetTriangle();

    // モデル
    SetModel();
}