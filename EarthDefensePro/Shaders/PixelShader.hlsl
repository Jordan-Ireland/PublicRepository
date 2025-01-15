#pragma pack_matrix(row_major)

struct OBJ_ATTRIBUTES
{
    float3 Kd; // : SV_TARGET; // diffuse reflectivity / remove sv_target for 4c
    float d; // dissolve (transparency) 
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
    uint illum; // illumination model
};

struct Point_Light
{
    float4x4 transform;
    float4 color;
    float watts;
    float cutoff_distance;
    float pad1, pad2;
};

struct SPOT_LIGHT
{
    float4x4 transform;
    float4 color;
    float watts;
    float cutoff_distance;
    float padding1, padding2;
};

cbuffer CB_Object : register(b0)
{
    float4x4 cbViewMatrix;
    float4x4 cbProjectionMatrix;
    float4 materialIndex;
    min16uint useTexture;
}

cbuffer CB_Frame : register(b1)
{
    float4 sunLightColor;
    float3 sunLightDir;
    float time;
    Point_Light plights[16];
    SPOT_LIGHT slights[16];
};

cbuffer CB_Scene : register(b2)
{
    OBJ_ATTRIBUTES attributes[150];
    float4 ambient;
    float pointlights;
    float spotlights;
    float pad1, pad2;
}

Texture2D diffuseT : register(t0);
Texture2D<float4> specularT : register(t1);
Texture2D ambientT : register(t2);
Texture2D shininessT : register(t3);
SamplerState filter : register(s0);

struct OutputToRasterizer
{
    float4 posH : SV_POSITION;
    float3 uvw : UVW;
    float3 normW : NORM;
    float3 posW : WORLDPOS;
    float3 cameraPos : WORLDCAMPOS;
};

float4 main(OutputToRasterizer vIn) : SV_TARGET
{
   
    //float4 surfaceColor = float4(attributes[materialIndex.x].Kd, 1.0f); // USING THE MATERIAL COLOR 
    float4 surfaceColor = float4(attributes[materialIndex.x].Kd, 1); // USING THE TEXTURE COLOR
    if ((bool) useTexture)
    {
        surfaceColor = diffuseT.Sample(filter, vIn.uvw);
    }
    
    //check specular
    float4 specularMat = specularT.Sample(filter, vIn.uvw);
    if (all(specularMat == float4(0, 0, 0, 0)))
    {
        specularMat = float4(attributes[materialIndex.x].Ks, 1);
    }
    
    //check ambient
    float4 ambientMat = ambientT.Sample(filter, vIn.uvw);
    if (all(ambientMat == float4(0, 0, 0, 0)))
    {
        ambientMat = float4(attributes[materialIndex.x].Ka, 1);
    }
    
    //check shine
    float4 shineMat = shininessT.Sample(filter, vIn.uvw);
    if (all(shineMat == float4(0, 0, 0, 0)))
    {
        shineMat = float4(attributes[materialIndex.x].Ke, 1);
    }
    
    //surfaceColor = ambientMat + surfaceColor;

    
    // Ambient and Directional Lighting
    float4 amb = ambient * surfaceColor;
    float4 directionalLight = saturate(dot(normalize(-sunLightDir), vIn.normW)) * sunLightColor * surfaceColor;
    float4 color = directionalLight + amb;

    // Point Lights
    if (pointlights > 0)
    {
        float4 pointLightColor = float4(0, 0, 0, 0);

        // For each point light in scene
        for (int i = 0; i < pointlights; i++)
        {
            float3 lightPos = plights[i].transform._41_42_43;

            // Pointlight Maths
            float3 lightDir = normalize(lightPos - vIn.posW);
            float lightRatio = saturate(dot(normalize(lightDir), normalize(vIn.normW)));
            float attenuation = 1.0f - saturate(length(lightPos - vIn.posW) / plights[i].cutoff_distance);
            pointLightColor = lightRatio * plights[i].color * (plights[i].watts * 0.01f) * surfaceColor * attenuation;
            color += pointLightColor;
        }
        
    }
    
    // Spotlights
    if (spotlights > 0)
    {
        float4 spotLightColor = float4(0, 0, 0, 0);
        float coneInnerRatio = 0.92f;
        float coneOuterRatio = 0.9;

        for (int i = 0; i < spotlights; i++)
        {
            float3 lightPos = slights[i].transform._41_42_43;

            // Cone Direction
            float4x4 identity = mul(slights[i].transform, float4x4(cos(time), 0, sin(time), 0,
                0, 1, 0, 0,
                -sin(time), 0, cos(time), 0,
                0, 0, 0, 1));
            float3 coneDir = normalize(identity._31_32_33);

            // Spotlight maths
            float3 lightDir = normalize(lightPos - vIn.posW);
            float surfaceRatio = saturate(dot(-lightDir, normalize(coneDir)));
            float spotFactor = 1 - saturate((coneInnerRatio - surfaceRatio) / (coneInnerRatio - coneOuterRatio));
            float lightRatio = saturate(dot(normalize(lightDir), normalize(vIn.normW)));
            float attenuation = 1.0f - saturate(length(lightPos - vIn.posW) / slights[i].cutoff_distance);
            
            spotLightColor += lightRatio * slights[i].color * (slights[i].watts * 0.01f) * surfaceColor * attenuation * spotFactor;
            color += spotLightColor;
        }
    }

    // Specular
    float3 viewDir = normalize(vIn.cameraPos - vIn.posW);
    float3 halfVec = normalize((-sunLightDir) + viewDir);
    float dotProduct = max(0.0f, dot(vIn.normW, halfVec));
    float intensity = pow(saturate(dotProduct), attributes[materialIndex.x].Ns) + 0.000001f;

    return color += (specularMat * intensity);
    //return tcolor.Sample(filter, vIn.uvw) + color + (float4(attributes[materialIndex.x].Ks, 1.0f) * intensity);
}