

#if defined(USE_SKINNING)
mat4 getSkinningMatrix()
{
    mat4 skin = mat4(0);

    #if defined(HAS_WEIGHT_SET1) && defined(HAS_JOINT_SET1)
    skin +=
        a_Weight1.x * u_jointMatrix[int(a_Joint1.x)] +
        a_Weight1.y * u_jointMatrix[int(a_Joint1.y)] +
        a_Weight1.z * u_jointMatrix[int(a_Joint1.z)] +
        a_Weight1.w * u_jointMatrix[int(a_Joint1.w)];
    #endif

    #if defined(HAS_WEIGHT_SET2) && defined(HAS_JOINT_SET2)
    skin +=
        a_Weight2.x * u_jointMatrix[int(a_Joint2.x)] +
        a_Weight2.y * u_jointMatrix[int(a_Joint2.y)] +
        a_Weight2.z * u_jointMatrix[int(a_Joint2.z)] +
        a_Weight2.w * u_jointMatrix[int(a_Joint2.w)];
    #endif

    return skin;
}

mat4 getSkinningNormalMatrix()
{
    mat4 skin = mat4(0);

    #if defined(HAS_WEIGHT_SET1) && defined(HAS_JOINT_SET1)
    skin +=
        a_Weight1.x * u_jointNormalMatrix[int(a_Joint1.x)] +
        a_Weight1.y * u_jointNormalMatrix[int(a_Joint1.y)] +
        a_Weight1.z * u_jointNormalMatrix[int(a_Joint1.z)] +
        a_Weight1.w * u_jointNormalMatrix[int(a_Joint1.w)];
    #endif

    #if defined(HAS_WEIGHT_SET2) && defined(HAS_JOINT_SET2)
    skin +=
        a_Weight2.x * u_jointNormalMatrix[int(a_Joint2.x)] +
        a_Weight2.y * u_jointNormalMatrix[int(a_Joint2.y)] +
        a_Weight2.z * u_jointNormalMatrix[int(a_Joint2.z)] +
        a_Weight2.w * u_jointNormalMatrix[int(a_Joint2.w)];
    #endif

    return skin;
}
#endif // !USE_SKINNING

#if defined(USE_MORPHING)
vec4 getTargetPosition()
{
    vec4 pos = vec4(0);

#if defined(HAS_TARGET_POSITION0)
    pos.xyz += u_morphWeights[0] * a_Target_Position0;
#endif

#if defined(HAS_TARGET_POSITION1)
    pos.xyz += u_morphWeights[1] * a_Target_Position1;
#endif

#if defined(HAS_TARGET_POSITION2)
    pos.xyz += u_morphWeights[2] * a_Target_Position2;
#endif

#if defined(HAS_TARGET_POSITION3)
    pos.xyz += u_morphWeights[3] * a_Target_Position3;
#endif

#if defined(HAS_TARGET_POSITION4)
    pos.xyz += u_morphWeights[4] * a_Target_Position4;
#endif

    return pos;
}

vec3 getTargetNormal()
{
    vec3 normal = vec3(0);

#if defined(HAS_TARGET_NORMAL0)
    normal += u_morphWeights[0] * a_Target_Normal0;
#endif

#if defined(HAS_TARGET_NORMAL1)
    normal += u_morphWeights[1] * a_Target_Normal1;
#endif

#if defined(HAS_TARGET_NORMAL2)
    normal += u_morphWeights[2] * a_Target_Normal2;
#endif

#if defined(HAS_TARGET_NORMAL3)
    normal += u_morphWeights[3] * a_Target_Normal3;
#endif

#if defined(HAS_TARGET_NORMAL4)
    normal += u_morphWeights[4] * a_Target_Normal4;
#endif

    return normal;
}

vec3 getTargetTangent()
{
    vec3 tangent = vec3(0);

#if defined(HAS_TARGET_TANGENT0)
    tangent += u_morphWeights[0] * a_Target_Tangent0;
#endif

#if defined(HAS_TARGET_TANGENT1)
    tangent += u_morphWeights[1] * a_Target_Tangent1;
#endif

#if defined(HAS_TARGET_TANGENT2)
    tangent += u_morphWeights[2] * a_Target_Tangent2;
#endif

#if defined(HAS_TARGET_TANGENT3)
    tangent += u_morphWeights[3] * a_Target_Tangent3;
#endif

#if defined(HAS_TARGET_TANGENT4)
    tangent += u_morphWeights[4] * a_Target_Tangent4;
#endif

    return tangent;
}

#endif // !USE_MORPHING
