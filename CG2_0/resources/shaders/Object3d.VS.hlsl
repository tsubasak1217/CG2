#include "Object3D.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 world;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrixTriangle : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixTriangle.WVP);
    output.color = input.color;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrixTriangle.world));
    return output;
}