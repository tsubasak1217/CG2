#include "Object3D.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    int enebleLighting;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

StructuredBuffer<Material> gMaterial : register(t0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);
Texture2D<float4> gTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial[input.instanceID].uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    PixelShaderOutput output;
    
    if (gMaterial[input.instanceID].enebleLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

        output.color = gMaterial[input.instanceID].color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        
    }
    else
    {
        output.color = gMaterial[input.instanceID].color * textureColor;
    }
    
    return output;
}
