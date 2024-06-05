#pragma once
#include <stdint.h>
#include <vector>
#include "Vector4.h"
#include "VertexData.h"
#include "Material.h"
#include "Transform.h"

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;

struct TriangleData{
    std::vector<VertexData>vertices;
    std::vector<Material>colorf;
    std::vector<TransformMatrix>transform;
};


class PolygonManager{

public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void Finalize();
    void Reset();
    void DrawPolygonAll();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        bool useTexture, bool view3D
    );

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上弦や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 32768;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;

private:// 実際に頂点情報や色などの情報が入っている変数

    TriangleData triangles_;

private:// Resourceを格納する変数

    ID3D12Resource* vertexResource_;
    ID3D12Resource* materialResource_;
    ID3D12Resource* wvpResource_;

private:

    int textureNum_ = 1;
};