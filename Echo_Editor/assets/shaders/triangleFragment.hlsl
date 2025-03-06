struct PSInput
{
    float3 inColor : COLOR0;
};

float4 main(PSInput input) : SV_Target
{
    return float4(input.inColor, 1.0f);
}
