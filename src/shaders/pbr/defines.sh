
uniform vec4 u_LightDirection;
uniform vec4 u_LightColor;
uniform vec4 u_AmbientLightColor;
uniform vec4 u_Camera;
uniform vec4 u_EmissiveFactor;
uniform vec4 u_BaseColorFactor;
uniform vec4 u_MetallicRoughnessValues;

// debugging flags used for shader output of intermediate PBR variables
uniform vec4 u_ScaleDiffBaseMR;
uniform vec4 u_ScaleFGDSpec;
uniform vec4 u_ScaleIBLAmbient;

SAMPLERCUBE(u_DiffuseEnvSampler, 0);
SAMPLERCUBE(u_SpecularEnvSampler, 1);
SAMPLER2D(u_brdfLUT, 2);
SAMPLER2D(u_BaseColorSampler, 3);
SAMPLER2D(u_NormalSampler, 4);
SAMPLER2D(u_EmissiveSampler, 5);
SAMPLER2D(u_MetallicRoughnessSampler, 6);
SAMPLER2D(u_OcclusionSampler, 7);

// x=Base, y=Normal, z=Emissive, w=M/R 
uniform vec4 u_HasTable0;
// x=Occlusion, y=Tangents, z=Normals, w=UV
uniform vec4 u_HasTable1;
// x=LightBlinnPhong, y=LightLambert, z=LightPBR, w=Null
uniform vec4 u_UseLightTable;

// x=IBL, y=TexLoD, z=OcclusionStrength, w=NormalScale
uniform vec4 u_Data0;

#define u_hasBase u_HasTable0.x
#define u_hasNormalMap u_HasTable0.y
#define u_hasEmissive u_HasTable0.z
#define u_hasMetal u_HasTable0.w

#define u_hasOcclusion u_HasTable1.x
#define u_hasTangents u_HasTable1.y
#define u_hasNormals u_HasTable1.z
#define u_hasUV u_HasTable1.w

#define u_IBL u_Data0.x
#define u_TexLoD u_Data0.y
#define u_OcclusionStrength u_Data0.z
#define u_NormalScale u_Data0.w

#define u_LightBlinnPhong u_UseLightTable.x
#define u_LightLambert u_UseLightTable.y
#define u_LightPBR u_UseLightTable.z