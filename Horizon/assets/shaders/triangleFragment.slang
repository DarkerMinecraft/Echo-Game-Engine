struct PSInput 
{
	float3 color : COLOR;
};

[shader("pixel")]
float4 main(PSInput input) : SV_Target
{
    return float4(input.color, 1.0);
}