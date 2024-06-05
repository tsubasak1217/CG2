struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    int useTexture : USE_TEXTURE0;
};