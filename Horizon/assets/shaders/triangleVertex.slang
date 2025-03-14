struct VS_INPUT
{
    float2 position : ATTRIB0;
    float3 color : ATTRIB1;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

[shader("vertex")]
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 0.0, 1.0);
    output.color = input.color;
    return output;
}
