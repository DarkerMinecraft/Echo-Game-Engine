struct VSInput
{
    float3 worldPosition;
    float3 localPosition;

    float4 color;
    float outlineThickness;
    float fade;

    int instanceID;
}

struct VSOuput
{
    float4 position : SV_POSITION;
    float2 localPosition;
    float4 color;
    float outlineThickness;
    float fade;

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
    output.position = mul(float4(input.worldPosition, 1.0), cam.projViewMatrix);
    output.localPosition = input.localPosition.xy;
    output.color = input.color;
    output.outlineThickness = input.outlineThickness;
    output.fade = input.fade;
    output.instanceID = input.instanceID;

    return output;
}

struct PSInput
{
    float2 localPosition;
    float4 color;
    float outlineThickness;
    float fade;

    nointerpolation int instanceID;
}

struct PSOutput
{
    float4 color : SV_Target;
    int instanceID : SV_Target1;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input)
{
    PSOutput output;

    float distance = 1.0 - length(input.localPosition);
    float circleAlpha = smoothstep(0.0, input.fade, distance);
    circleAlpha *= smoothstep(input.outlineThickness + input.fade, input.outlineThickness, distance);

    output.instanceID = input.instanceID;
    output.color = input.color;
    output.color.a *= circleAlpha;
    return output;
}