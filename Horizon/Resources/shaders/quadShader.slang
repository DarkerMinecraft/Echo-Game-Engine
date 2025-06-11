struct VSInput
{
    float3 position;
    float2 uv;
    float4 color;
    int texIndex;
    float tilingFactor;
    int instanceID;
}

struct VSOuput
{
    float4 position : SV_POSITION;
    float2 uv;
    float4 color;
    nointerpolation int texIndex;
    float tilingFactor;
    nointerpolation int instanceID;
}

struct Camera 
{
    float4x4 projViewMatrix; 
}

[[vk::binding(0, 0)]] ConstantBuffer<Camera> cam : register(b0);

[shader("vertex")]
VSOuput vertexMain(VSInput input)
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

struct PSInput
{
    float2 uv;
    float4 color;
    nointerpolation int texIndex;
    float tilingFactor;
    nointerpolation int instanceID;
}

[[vk::binding(1, 0)]] Sampler2D texSamplers[] : register(s1, space0);

struct PSOutput
{
    float4 color : SV_Target;
    int instanceID : SV_Target1;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input)
{
    PSOutput output;

    float4 texColor = texSamplers[input.texIndex].Sample(input.uv * input.tilingFactor);
    output.color = input.color * texColor;

    output.instanceID = input.instanceID;
    return output;
}