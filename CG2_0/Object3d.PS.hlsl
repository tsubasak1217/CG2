#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enebleLighting;
    float4x4 uvTransform;
};

struct DirectionalLight
{
    float4 color_;
    float3 direction_;
    float intensity;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f,1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    
    if (gMaterial.enebleLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction_);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        output.color = gMaterial.color * textureColor * gDirectionalLight.color_ * cos * gDirectionalLight.intensity;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}
