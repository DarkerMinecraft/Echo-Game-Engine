cbuffer StarData : register(b0) 
{
    float4 data1;
};

RWTexture2D<float4> outputImage : register(u1);

// Random noise in the range [0.0, 1.0]
float Noise2d(float2 x) {
    float xhash = cos(x.x * 37.0);
    float yhash = cos(x.y * 57.0);
    return frac(415.92653 * (xhash + yhash));
}

// Convert Noise2d into a "star field" by thresholding
float NoisyStarField(float2 vSamplePos, float fThreshold) {
    float StarVal = Noise2d(vSamplePos);
    if (StarVal >= fThreshold) {
        StarVal = pow((StarVal - fThreshold) / (1.0 - fThreshold), 6.0);
    } else {
        StarVal = 0.0;
    }
    return StarVal;
}

// Stabilized star field using linear interpolation
float StableStarField(float2 vSamplePos, float fThreshold) {
    float2 fractCoord = frac(vSamplePos);
    float2 floorSample = floor(vSamplePos);

    float v1 = NoisyStarField(floorSample, fThreshold);
    float v2 = NoisyStarField(floorSample + float2(0.0, 1.0), fThreshold);
    float v3 = NoisyStarField(floorSample + float2(1.0, 0.0), fThreshold);
    float v4 = NoisyStarField(floorSample + float2(1.0, 1.0), fThreshold);

    float StarVal =
        v1 * (1.0 - fractCoord.x) * (1.0 - fractCoord.y) +
        v2 * (1.0 - fractCoord.x) * fractCoord.y +
        v3 * fractCoord.x * (1.0 - fractCoord.y) +
        v4 * fractCoord.x * fractCoord.y;

    return StarVal;
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID) {
    uint2 texelCoord = dispatchThreadID.xy;

    // Assuming a predefined screen resolution (e.g., set externally or hardcoded)
    uint2 resolution = uint2(1920, 1080); // Replace with the desired resolution

    if (texelCoord.x < resolution.x && texelCoord.y < resolution.y) {
        float2 fragCoord = float2(texelCoord);

        // Sky background color
        float3 vColor = data1.xyz * fragCoord.y / resolution.y;

        // Star field threshold
        float StarFieldThreshold = data1.w;

        // Stars with slow crawl
        float2 vSamplePos = fragCoord + float2(0.2, -0.06);
        float StarVal = StableStarField(vSamplePos, StarFieldThreshold);

        vColor += float3(StarVal, StarVal, StarVal);

        float4 fragColor = float4(vColor, 1.0);
        outputImage[texelCoord] = fragColor;
    }
}
