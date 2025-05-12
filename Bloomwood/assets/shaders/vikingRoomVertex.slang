struct VSInput 
{

    float3 position : ATTRIB0;
    float2 texCoords : ATTRIB1;
    float3 normal : ATTRIB2;

}

struct VSOutput 
{

    float4 position : SV_Position;
    float2 texCoords : TEXCOORD0;
    float3 normal : NORMAL0;

}

[[vk::binding(0, 0)]] cbuffer Camera : register(b0)
{
    float4x4 projViewMatrix;
}

[[vk::binding(1, 0)]] cbuffer Tranform : register(b1)
{
    float4x4 transform;
}

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(transform, float4(input.position, 1.0));
    output.position = mul(projViewMatrix, worldPos);

    output.texCoords = input.texCoords;
    output.normal = input.normal;
    return output;
}