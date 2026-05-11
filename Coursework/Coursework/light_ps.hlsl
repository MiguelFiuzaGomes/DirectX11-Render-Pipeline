// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColour[3];
    float4 ambientColour[3];
    float3 lightDirection[3];
    float3 lightPosition[3];
    float2 padding;
};

cbuffer attenuationBuffer : register(b1)
{
    float constantAtt;
    float linearAtt;
    float quadraticAtt;
    float padding1;
};

cbuffer screenSizeBuffer : register(b2)
{
    float width;
    float height;
    float2 screenPadding;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
};

// Calculate the attenuation for the point lights
float calculateAttenuation(float3 lightPos, float3 worldPos)
{
    float distance = length(lightPos - worldPos);
    float attenuation = 10.f / (2 * (constantAtt + (linearAtt * distance) + (distance * distance)));
    
    return attenuation;
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse, float4 ambient)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    
    colour += (ambient * 0.5);
    
    return colour;
}

// Calculate lighting for point lights
float4 calculatePointLighting(float3 lightPos, InputType input, float4 diffuseColour, float4 ambientColour)
{
    float4 colour;
    
    float3 worldPos = input.worldPosition;
    float attenuation = calculateAttenuation(lightPos, worldPos);

    float3 lightDir = normalize(lightPos - worldPos);
    
    float intensity = saturate(dot(input.normal, lightDir));
    
    colour = saturate(intensity * diffuseColour);
    colour = saturate(colour + (ambientColour * attenuation));
    
    return colour;

}

// Calculate the amount of bias based on the slope (Slope Scale Depth Bias) 
float calculateSlopeDepthBias(float3 lightDir, float3 normal, float constantBias, float slopeFactor)
{
    float bias;
    float slope;
    
    // Calculate the dot product between the surface normal and the light direction
    slope = abs(dot(normal, lightDir));
    
    
    // Apply the slope scale depth bias
    bias = constantBias + slopeFactor * slope;
    
    return bias;
}

// Check for depth data
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    
    return true;
}

// Check if the pixel is in shadow or not
bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPos, float3 lightDir, float3 normal, float constantBias, float slopeFactor)
{
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    
    float lightDepthValue = lightViewPos.z / lightViewPos.w;
    
    float bias = calculateSlopeDepthBias(lightDir, normal, constantBias, slopeFactor);

    
    lightDepthValue -= bias;
    
    if (lightDepthValue < depthValue)
        return false;
    
    return true;
}

float2 getProjectiveCoord(float4 lightViewPos)
{
    float2 projTex = lightViewPos.xy / lightViewPos.w;

    projTex *= float2(0.5f, -0.5f);
    projTex += 0.5f;
    
   // projTex = (projTex * 0.5f) + 0.5f;
    
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    // Variables
    // Sample the texture.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    
    // Shadows
    float depthValue = 0;
    float lightDepthValue = 0;
    
    float constBias = 0.005f; // constant bias (same as applied in class)
    float slopeFactor = 2.f; //  control the effect that a slope has on the bias
    
    // Light Colours
    float4 dirLightColour;
    float4 pointlightColour;
    float4 pointlightColour2;
    float4 finalColour;
    
    
    //Point Lights 
    pointlightColour = calculatePointLighting(lightPosition[0], input, diffuseColour[1], ambientColour[1]);
    pointlightColour2 = calculatePointLighting(lightPosition[1], input, diffuseColour[2], ambientColour[2]);
     
    finalColour = pointlightColour + pointlightColour2;
    
    // Calculate the projected texture coordinates
    float2 pTexCoord = getProjectiveCoord(input.lightViewPos);
    
    // Determine if the coords are in range, being outside of the light's view
    if (hasDepthData(pTexCoord))
        
        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, lightDirection[0], input.normal, constBias, slopeFactor))
        {
            //Directional Light
            dirLightColour = calculateLighting(-lightDirection[0], input.normal, diffuseColour[0], ambientColour[0]);

            // Join the directional light to the final lights colour
            finalColour += dirLightColour;

        }
    

    
    // Return final colour after lighting and applying the texture
    return finalColour * textureColour;
}