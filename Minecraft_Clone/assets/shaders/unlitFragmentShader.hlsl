struct PS_INPUT {
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}