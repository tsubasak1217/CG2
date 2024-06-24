#pragma once
#include <stdint.h>
#include <vector>
#include "Vector4.h"
#include "VertexData.h"
#include "Model.h"
#include "Material.h"
#include "Transform.h"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;

struct TriangleData{
    std::vector<VertexData>vertices;
    std::vector<Material>colorf;
    std::vector<TransformMatrix>transform;
    std::vector<uint32_t>GH;
};

struct Models{
    std::vector<ModelData>modelData;
    std::vector<Material>material;
    std::vector<TransformMatrix>transform;
    std::vector<uint32_t>GH;
};


class PolygonManager{

public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void Finalize();
    void Reset();

public:
    void DrawPolygonAll();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        bool useTexture, bool view3D, uint32_t GH
    );

    void AddModel(Model model,uint32_t GH);

private:

    void SetTriangle();
    void SetModel();

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上弦や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 32768;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;

private:// 実際に頂点情報や色などの情報が入っている変数

    TriangleData triangles_;
    Models modelPolygons_;

private:// Resourceを格納する変数

    ComPtr<ID3D12Resource> vertexResource_ = nullptr;
    ComPtr<ID3D12Resource> materialResource_ = nullptr;
    ComPtr<ID3D12Resource> wvpResource_ = nullptr;

    ComPtr<ID3D12Resource> vertexResource_model_ = nullptr;
    ComPtr<ID3D12Resource> materialResource_model_ = nullptr;
    ComPtr<ID3D12Resource> wvpResource_model_ = nullptr;

private:

    int textureNum_ = 1;
};