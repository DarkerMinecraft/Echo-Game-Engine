struct VSInput
{
    float3 position;
    float4 color;
}

struct VSOuput
{
    float4 position : SV_POSITION;
    float4 color;
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
    output.color = input.color;

    return output;
}

struct PSInput
{
    float4 color;
}

struct PSOutput
{
    float4 color : SV_Target;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input)
{
    PSOutput output;

    output.color = input.color;

    return output;
}