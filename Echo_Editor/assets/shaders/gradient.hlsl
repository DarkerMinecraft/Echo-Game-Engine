// HLSL Compute Shader equivalent

// Bind the UAV - note that while GLSL specifies rgba16f, here we use float4.
// If you need 16-bit precision you might consider using half4 or similar.
RWTexture2D<float4> image : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID, 
          uint3 groupThreadID    : SV_GroupThreadID)
{
    // Convert the global thread id to integer coordinates
    int2 texelCoord = int2(dispatchThreadID.xy);

    // Get the dimensions of the texture
    int width, height;
    image.GetDimensions(width, height);
    int2 size = int2(width, height);

    // Ensure we do not access out-of-bounds texels
    if (texelCoord.x < size.x && texelCoord.y < size.y)
    {
        float4 color = float4(0.0, 0.0, 0.0, 1.0);

        // Check that the local invocation id in x and y are non-zero
        if (groupThreadID.x != 0 && groupThreadID.y != 0)
        {
            color.x = float(texelCoord.x) / float(size.x);
            color.y = float(texelCoord.y) / float(size.y);
        }

        // Write the color to the output image at the computed texel coordinate
        image[texelCoord] = color;
    }
}
