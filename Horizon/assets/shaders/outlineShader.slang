struct VSOutput 
{
    float4 position : SV_Position;
    float2 uv;
}

[shader("vertex")]
VSOutput vertexMain(uint vertexID : SV_VertexID) 
{
    VSOutput output;

    // Define positions for a full-screen triangle
    float2 positions[3] = {
        float2(-1.0f, -1.0f),
        float2( 3.0f, -1.0f),
        float2(-1.0f,  3.0f)
    };

    // Define corresponding UV coordinates
    float2 uvs[3] = {
        float2(0.0f, 0.0f),
        float2(2.0f, 0.0f),
        float2(0.0f, 2.0f)
    };

    output.position = float4(positions[vertexID], 0.0f, 1.0f);
    output.uv = uvs[vertexID];
    return output;
}

struct PSInput
{
    float2 uv;
};

struct OutlineParams
{
    int selectedEntityID;
    float4 outlineColor;
    float outlineThickness;
};

[[vk::binding(0, 0)]] Sampler2D colorTexture : register(s0);
[[vk::binding(1, 0)]] Texture2D<int> idTexture : register(t1);
[[vk::binding(2, 0)]] ConstantBuffer<OutlineParams> params : register(b2);

struct PSOutput 
{
    float4 color : SV_Target;
}

[shader("pixel")]
PSOutput pixelMain(PSInput input)
{
    
    PSOutput output;

    uint width, height;
    colorTexture.GetDimensions(width, height);

    // Convert UV to pixel coordinates
    int2 pixelCoords = int2(input.uv * float2(width, height));
    int centerID = idTexture.Load(int3(pixelCoords, 0)).r;

    float4 color = colorTexture.Sample(input.uv);
    
    bool isSelected = (centerID == params.selectedEntityID);
    bool hasOutline = false;

    if (!isSelected)
    {
        int2 offsets[8] = {
            int2(-1, -1), int2(0, -1), int2(1, -1),
            int2(-1, 0),               int2(1, 0),
            int2(-1, 1),  int2(0, 1),  int2(1, 1)
        };

        int thickness = int(params.outlineThickness);

        for (int i = 0; i < 8; i++)
        {
            int2 neighborPos = pixelCoords + offsets[i] * thickness;

            if (neighborPos.x >= 0 && neighborPos.y >= 0 &&
                neighborPos.x < width && neighborPos.y < height)
            {
                int neighborID = idTexture.Load(int3(neighborPos, 0)).r;
                if (neighborID == params.selectedEntityID)
                {
                    hasOutline = true;
                    break;
                }
            }
        }
    }

    output.color = hasOutline ? params.outlineColor : color;
    return output;
}
