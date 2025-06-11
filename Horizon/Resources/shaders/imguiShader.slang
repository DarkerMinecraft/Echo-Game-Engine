struct VSInput
{
    float2 pos;
    float4 col;
    float2 uv;
}

struct VSOutput
{
    float4 pos : SV_Position;
    float4 col;
    float2 uv;
}

struct VertexBuffer
{
    float4x4 projectionMatrix;
    float2 framebufferScale;
}

[[vk::binding(0, 0)]] ConstantBuffer<VertexBuffer> vertexBuffer : register(b0);

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    float2 scaledPos = input.pos * vertexBuffer.framebufferScale;
    output.pos = mul(float4(scaledPos, 0.0f, 1.0f), vertexBuffer.projectionMatrix);

    output.col = input.col;
    output.uv = input.uv;

    return output;
}

struct PSInput
{
    float4 col;
    float2 uv;
}

[[vk::binding(1, 0)]] Sampler2D texture : register(t1, s0);

[shader("pixel")]
float4 pixelMain(PSInput input) : SV_Target
{
    return input.col * texture.Sample(input.uv);
}