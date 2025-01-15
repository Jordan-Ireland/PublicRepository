// an ultra simple hlsl vertex shader
#pragma pack_matrix(row_major)

cbuffer CB_Object : register(b0)
{
    float4x4 cbViewMatrix;
    float4x4 cbProjectionMatrix;
    float4 materialIndex;
    min16uint useTexture;
};
struct mat
{
    float4x4 matInstance;
};

struct InputVertex
{
    float3 pos : POSITION;
    float3 uvw : UVW;
    float3 norm	: NORM;
    //float4x4 worldMatrix : WORLD;
};

struct OutputToRasterizer
{
    float4 posH : SV_POSITION;
    float3 uvw : UVW;
    float3 normW : NORM;
    float3 posW : WORLDPOS;
    float3 cameraPos : WORLDCAMPOS;
};

StructuredBuffer<mat> matrixData : register(t2);

//OutputToRasterizer main(InputVertex vIn)
//{
//    OutputToRasterizer vOut;
//     Save world position
//    vOut.posW = mul(float4(vIn.pos, 1.0f), vIn.worldMatrix).xyz;

//     Save normal in world space
//    vOut.normW = normalize(mul(vIn.norm, (float3x3) vIn.worldMatrix));

//     Save camera position in world space
//    vOut.cameraPos = -float3(cbViewMatrix._m30, cbViewMatrix._m31, cbViewMatrix._m32);

//    vOut.uvw = vIn.uvw;

//     put vertex in homogenous space
//    vOut.posH = float4(vIn.pos, 1.0f);
//    vOut.posH = mul(vOut.posH, vIn.worldMatrix);
//    vOut.posH = mul(vOut.posH, cbViewMatrix);
//    vOut.posH = mul(vOut.posH, cbProjectionMatrix);

//    return vOut;
//}

OutputToRasterizer main(InputVertex vIn, uint instance : SV_InstanceID)
{
    OutputToRasterizer vOut;

    // Fetch the matrix for this instance
    float4x4 instanceMatrix = matrixData[instance + int(materialIndex.z)].matInstance;
    
 
   
    // Save world position
    vOut.posW = mul(float4(vIn.pos, 1.0f), instanceMatrix).xyz;

    // Save normal in world space
    vOut.normW = normalize(mul(vIn.norm, (float3x3) instanceMatrix));

    // Save camera position in world space
    vOut.cameraPos = -float3(cbViewMatrix._m30, cbViewMatrix._m31, cbViewMatrix._m32);

    vOut.uvw = vIn.uvw;

    // put vertex in homogenous space
    vOut.posH = float4(vIn.pos, 1.0f);
    vOut.posH = mul(vOut.posH, instanceMatrix);
    vOut.posH = mul(vOut.posH, cbViewMatrix);
    vOut.posH = mul(vOut.posH, cbProjectionMatrix);

    return vOut;
}