struct VSInput 
{

    float3 position;
    float2 texCoords;
    float3 normals;

}

struct VSOutput 
{

    float4 position : SV_Position;
    float2 texCoords;
    float3 normals;

}

struct ModelBuffer 
{
    float4x4 modelMatrix;
}

struct CameraBuffer 
{
    float4x4 viewProjectionMatrix;
}

[vk::binding(0, 0)] ConstantBuffer<ModelBuffer> modelBuffer;
[vk::binding(1, 0)] ConstantBuffer<CameraBuffer> cameraBuffer;

[shader("vertex")]
VSOutput vertexMain(VSInput input) 
{
    VSOutput output;

    output.position = mul(float4(input.position, 1.0), modelBuffer.modelMatrix * cameraBuffer.viewProjectionMatrix);
    output.texCoords = input.texCoords;
    output.normals = input.normals;

    return output;
}

struct PSInput 
{
    float2 texCoords;
    float3 normals;
}

struct PSOutput 
{
    float4 color : SV_Target;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input) 
{
    PSOutput output;

    output.color = float4(1.0, 0.0, 0.0, 1.0);

    return output;
}