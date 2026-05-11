// Pixel Shader responsible for Edge detection
// The method used is Sobel Edge Detection

Texture2D texture0 : register(t0);

SamplerState sample0 : register(s0);

cbuffer ScreenSize : register(b0)
{
    float screenW;
    float screenH;
    float2 padding;
};

struct InputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

static const float ConvulutionX[] =
{
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
    
};
static const float ConvulutionY[] =
{
    -1, -2, -1,
    0, 0, 0,
    1, 2, 1
};


float CheckEdge(float4 pixels[9])
{
    float3 inX = (0.f, 0.f, 0.f);
    float3 inY = (0.f, 0.f, 0.f);
    
    for (int i = 0; i < 9; i++)
    {
        float4 pixel = pixels[i];
        
        inX += pixel.rgb * ConvulutionX[i];
        inY += pixel.rgb * ConvulutionX[i];
    }

    return length(inX) + length(inY);
}


float4 main(InputType input) : SV_Target
{
    float maxDifference = 1.5f;
    float4 textureColour = texture0.Sample(sample0, input.tex);
    
    int i = 0; // iterator for the texture
    float4 pixels[9];
    
    float2 texelSize = float2(1.f / screenW, 1.f / screenH);
    
    // run through the Y coords
    for (int j = -1; j < 2; j++)
    {
        // run through the X coords
        for (int x = -1; x < 2; x++)
        {
            pixels[i] = texture0.Sample(sample0, input.tex + float2(i, j) * texelSize);
            i++;
        }

    }

    float edge = CheckEdge(pixels);
    
    if(edge >= maxDifference)
        return float4(0.898, 0.360, 0.219, 1.f);;
    
    return textureColour;
    
}