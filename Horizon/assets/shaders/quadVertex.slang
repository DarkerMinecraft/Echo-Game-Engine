struct VSInput
{
    float3 position : ATTRIB0;
    float2 uv : ATTRIB1;
    float4 color : ATTRIB2;
    int texIndex : ATTRIB3;
    float tilingFactor : ATTRIB4;
    int instanceID : ATTRIB5;
}

struct VSOuput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    nointerpolation int texIndex : ATTRIB0;
    float tilingFactor : ATTRIB1;
    nointerpolation int instanceID : ATTRIB2;
}

struct Camera 
{
    float4x4 projViewMatrix; 
}

[[vk::binding(0, 0)]] ConstantBuffer<Camera> cam : register(b0);

[shader("vertex")]
VSOuput main(VSInput input)
{
    VSOuput output;
    output.position = mul(float4(input.position, 1.0), cam.projViewMatrix);
    output.uv = input.uv;
    output.color = input.color;
    output.texIndex = input.texIndex;
    output.tilingFactor = input.tilingFactor;
    output.instanceID = input.instanceID;

    return output;
}