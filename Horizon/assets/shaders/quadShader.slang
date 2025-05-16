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
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    int texIndex : ATTRIB0;
    float tilingFactor : ATTRIB1;
    int instanceID : ATTRIB2;
}

[[vk::binding(0, 1)]] Sampler2D texSamplers[] : register(s0, space1);

struct PSOutput
{
    float4 color : SV_Target;
    int instanceID : SV_Target1;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input) : SV_Target
{
    PSOutput output;

    // Sample the texture
    float4 texColor = texSamplers[input.texIndex].Sample(input.uv * input.tilingFactor);
    output.color = input.color * texColor;

    output.instanceID = input.instanceID;
    return output;
}