[[vk::binding(0, 0)]] RWTexture2D<float4> image : register(t0, space0);

[shader("compute")]
[numthreads(16,16,1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID, 
			uint3 groupThreadID: SV_GroupThreadID)
{
    int2 texelCoord = int2(dispatchThreadID.xy);

    int width, height;
    image.GetDimensions(width, height);
    int2 size = int2(width, height);

    if (texelCoord.x < size.x && texelCoord.y < size.y)
    {
        float4 color = float4(0.0, 0.0, 0.0, 1.0);

        if (groupThreadID.x != 0 && groupThreadID.y != 0)
        {
            color.x = float(texelCoord.x) / float(size.x);
            color.y = float(texelCoord.y) / float(size.y);
        }

        image[texelCoord] = color;
    }
}