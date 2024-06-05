#include "Object3D.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 world;
};

//ConstantBuffer<TransformationMatrix> gTransformationMatrixTriangle : register(b0);

cbuffer TransformationBuffer : register(b0)
{
    TransformationMatrix gTransformationMatrixTriangle;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4x4 wvp : WVP0;
    float4x4 world : WORLD0;
    int useTexture : USE_TEXTURE0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, input.wvp);
    output.color = input.color;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) input.world));
    output.useTexture = input.useTexture;
    return output;
}
