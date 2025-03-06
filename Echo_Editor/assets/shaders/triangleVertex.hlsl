struct VSInput
{
    float2 position : POSITION; 
    float3 color    : COLOR0;    
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 outColor : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    output.position = float4(input.position, 0.0f, 1.0f);
    output.outColor = input.color;
    
    return output;
}