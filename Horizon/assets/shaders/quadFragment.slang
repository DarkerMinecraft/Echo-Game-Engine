struct PSInput 
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    int texIndex : ATTRIB0;
    float tilingFactor : ATTRIB1;
    int instanceID : ATTRIB2;
}

[[vk::binding(1, 0)]] Sampler2D texSamplers[] : register(s0, space0);

struct PSOutput
{
    float4 color : SV_Target;
    int instanceID : SV_Target1;
}

[shader("pixel")]
PSOutput main(PSInput input) : SV_Target
{
    PSOutput output;
    
    // Sample the texture
    float4 texColor = texSamplers[input.texIndex].Sample(input.uv * input.tilingFactor);
    output.color = input.color * texColor;
    
    output.instanceID = input.instanceID;
    return output;
}