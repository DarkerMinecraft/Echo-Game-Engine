struct PSInput 
{

    float3 position : ATTRIB0;
    float2 texCoords : ATTRIB1;
    float3 normal : ATTRIB2;

}

struct PSOutput 
{

    float4 color : SV_Target;

}

[[vk::binding(1, 0)]] Sampler2D texSampler : register(s0, space0);

[shader("pixel")]
PSOutput main(PSInput input) : SV_Target
{
    PSOutput output;
    output.color = texSampler.Sample(input.texCoords);
    
    return output;
}