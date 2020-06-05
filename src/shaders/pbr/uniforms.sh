
SAMPLER2D(u_NormalSampler, 0);
SAMPLER2D(u_EmissiveSampler, 1);
SAMPLER2D(u_OcclusionSampler, 2);
SAMPLER2D(u_BaseColorSampler, 3);
SAMPLER2D(u_MetallicRoughnessSampler, 4);
SAMPLER2D(u_DiffuseSampler, 5);
SAMPLER2D(u_SpecularGlossinessSampler, 6);
SAMPLER2D(u_GGXLUT, 7);
SAMPLER2D(u_CharlieLUT, 8);
SAMPLER2D(u_ClearcoatSampler, 9);
SAMPLER2D(u_ClearcoatRoughnessSampler, 10);
SAMPLER2D(u_ClearcoatNormalSampler, 11);
SAMPLER2D(u_SheenColorIntensitySampler, 12);
SAMPLER2D(u_MetallicRoughnessSpecularSampler, 13);
SAMPLER2D(u_SubsurfaceColorSampler, 14);
SAMPLER2D(u_SubsurfaceThicknessSampler, 15);
SAMPLER2D(u_ThinFilmLUT, 16);
SAMPLER2D(u_ThinFilmSampler, 17);
SAMPLER2D(u_ThinFilmThicknessSampler, 18);
SAMPLER2D(u_ThicknessSampler, 19);
SAMPLER2D(u_AnisotropySampler, 20);
SAMPLER2D(u_AnisotropyDirectionSampler, 21);

SAMPLERCUBE(u_LambertianEnvSampler, 22);
SAMPLERCUBE(u_GGXEnvSampler, 23);
SAMPLERCUBE(u_CharlieEnvSampler, 24);

uniform mat3 u_NormalUVTransform;
uniform mat3 u_EmissiveUVTransform;
uniform mat3 u_OcclusionUVTransform;
uniform mat3 u_BaseColorUVTransform;
uniform mat3 u_MetallicRoughnessUVTransform;
uniform mat3 u_SpecularGlossinessUVTransform;
uniform mat3 u_DiffuseUVTransform;
uniform mat3 u_ClearcoatUVTransform;
uniform mat3 u_ClearcoatRoughnessUVTransform;
uniform mat3 u_ClearcoatNormalUVTransform;
uniform mat3 u_SheenColorIntensityUVTransform;
uniform mat3 u_MetallicRougnessSpecularUVTransform;
uniform mat3 u_SubsurfaceColorUVTransform;
uniform mat3 u_SubsurfaceThicknessUVTransform;
uniform mat3 u_ThinFilmUVTransform;
uniform mat3 u_ThinFilmThicknessUVTransform;
uniform mat3 u_ThicknessUVTransform;
uniform mat3 u_AnisotropyUVTransform;
uniform mat3 u_AnisotropyDirectionUVTransform;
uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_NormalMatrix;

uniform vec4 u_UniformSet[20];

#define u_MipCount u_UniformSet[0].x
#define u_OcclusionStrength u_UniformSet[0].y
#define u_NormalScale u_UniformSet[0].z
#define u_Exposure u_UniformSet[0].w

#define u_SheenRoughness u_UniformSet[1].x
#define u_Anisotropy u_UniformSet[1].y
#define u_SubsurfaceScale u_UniformSet[1].z
#define u_SubsurfaceDistortion u_UniformSet[1].w

#define u_SubsurfacePower u_UniformSet[2].x
#define u_ThinFilmThicknessMinimum u_UniformSet[2].y
#define u_ThinFilmThicknessMaximum u_UniformSet[2].z
#define u_Thickness u_UniformSet[2].w

#define u_NormalUVSet u_UniformSet[3].x
#define u_EmissiveUVSet u_UniformSet[3].y
#define u_OcclusionUVSet u_UniformSet[3].z
#define u_BaseColorUVSet u_UniformSet[3].w

#define u_MetallicRoughnessUVSet u_UniformSet[4].x
#define u_DiffuseUVSet u_UniformSet[4].y
#define u_SpecularGlossinessUVSet u_UniformSet[4].z
#define u_ClearcoatUVSet u_UniformSet[4].w

#define u_ClearcoatRoughnessUVSet u_UniformSet[5].x
#define u_ClearcoatNormalUVSet u_UniformSet[5].y
#define u_SheenColorIntensityUVSet u_UniformSet[5].z
#define u_MetallicRougnessSpecularTextureUVSet u_UniformSet[5].w

#define u_SubsurfaceColorUVSet u_UniformSet[6].x
#define u_SubsurfaceThicknessUVSet u_UniformSet[6].y
#define u_ThinFilmUVSet u_UniformSet[6].z
#define u_ThinFilmThicknessUVSet u_UniformSet[6].w

#define u_ThicknessUVSet u_UniformSet[7].x
#define u_AnisotropyUVSet u_UniformSet[7].y
#define u_AnisotropyDirectionUVSet u_UniformSet[7].z
#define u_MetallicFactor u_UniformSet[7].w

#define u_RoughnessFactor u_UniformSet[8].x
#define u_GlossinessFactor u_UniformSet[8].y
#define u_SheenIntensityFactor u_UniformSet[8].z
#define u_ClearcoatFactor u_UniformSet[8].w

#define u_ClearcoatRoughnessFactor u_UniformSet[9].x
#define u_MetallicRoughnessSpecularFactor u_UniformSet[9].y
#define u_SubsurfaceThicknessFactor u_UniformSet[9].z
#define u_ThinFilmFactor u_UniformSet[9].w

#define u_Transmission u_UniformSet[10].x
#define u_AlphaCutoff u_UniformSet[10].y
#define u_IOR_and_f0 u_UniformSet[10].zw

#define u_BaseColorFactor u_UniformSet[11]
#define u_DiffuseFactor u_UniformSet[12]
#define u_SpecularFactor u_UniformSet[13].xyz
#define u_SheenColorFactor u_UniformSet[14].xyz
#define u_AnisotropyDirection u_UniformSet[15].xyz
#define u_SubsurfaceColorFactor u_UniformSet[16].xyz
#define u_AbsorptionColor u_UniformSet[17].xyz
#define u_Camera u_UniformSet[18].xyz
#define u_EmissiveFactor u_UniformSet[19].xyz

#if defined(USE_MORPHING)
uniform vec4 u_morphWeights[WEIGHT_COUNT];
#endif

#if defined(USE_SKINNING)
uniform mat4 u_jointMatrix[JOINT_COUNT];
uniform mat4 u_jointNormalMatrix[JOINT_COUNT];
#endif

#if defined(USE_PUNCTUAL)
/*
dx, dy, dz, r,
cr, cg, cb, i,
px, py, pz, ic,
oc, t, n/a, n/a
*/
uniform mat4 u_Lights[LIGHT_COUNT];
#endif