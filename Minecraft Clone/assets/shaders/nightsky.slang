// Gradient Night Sky Compute Shader (Slang)

// Push constant layout
[[vk::push_constant]]
struct PushConstants {
    float time;
    float intensity;
};

// Texture binding
RWTexture2D<float4> gOutput : register(u0);

// Main compute function
[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint width, height;
    gOutput.GetDimensions(width, height);

    if (DTid.x >= width || DTid.y >= height) {
        return;
    }

    PushConstants pc;
    float2 uv = float2(DTid.x, DTid.y) / float2(width, height);

    // Gradient calculation
    float gradient = smoothstep(0.0, 1.0, uv.y);

    // Night sky effect
    float3 color = lerp(float3(0.05, 0.05, 0.1), float3(0.0, 0.0, 0.0), gradient);
    
    // Twinkling stars (using simple noise)
    float starDensity = 200.0;
    float star = frac(sin(dot(uv * starDensity + pc.time * 0.1, float2(12.9898, 78.233))) * 43758.5453);
    star = step(0.998, star) * pc.intensity;

    // Combine stars with gradient
    color += star;

    // Write to output texture
    gOutput[DTid.xy] = float4(color, 1.0);
}
