
#include "../common/bgfx_shader.sh"
#include "uniforms.sh"
#include "animation.sh"

vec4 getPosition()
{
	vec4 pos = vec4(a_position, 1.0);

#if defined(USE_MORPHING)
	pos += getTargetPosition();
#endif

#if defined(USE_SKINNING)
	pos = getSkinningMatrix() * pos;
#endif

	return pos;
}

#if defined(HAS_NORMALS)
vec3 getNormal()
{
	vec3 normal = a_normal;

#if defined(USE_MORPHING)
	normal += getTargetNormal();
#endif

#if defined(USE_SKINNING)
	normal = mat3(getSkinningNormalMatrix()) * normal;
#endif

	return normalize(normal);
}
#endif

#if defined(HAS_TANGENTS)
vec3 getTangent()
{
	vec3 tangent = a_tangent.xyz;

#if defined(USE_MORPHING)
	tangent += getTargetTangent();
#endif

#if defined(USE_SKINNING)
	tangent = mat3(getSkinningMatrix()) * tangent;
#endif

	return normalize(tangent);
}
#endif

void main()
{
	vec4 pos = u_ModelMatrix * getPosition();
	v_position = vec3(pos.xyz) / pos.w;

#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
		vec3 tangent = getTangent();
		vec3 normalW = normalize(vec3(u_NormalMatrix * vec4(getNormal(), 0.0)));
		vec3 tangentW = normalize(vec3(u_ModelMatrix * vec4(tangent, 0.0)));
		vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
		mat3 tbn = mat3(tangentW, bitangentW, normalW);
		v_tbn0 = tbn[0];
		v_tbn1 = tbn[1];
		v_tbn2 = tbn[2];
#endif
#if defined(HAS_NORMALS) && !defined(HAS_TANGENTS)
		v_normal = normalize(vec3(u_NormalMatrix * vec4(getNormal(), 0.0)));
#endif // !HAS_NORMALS

	v_texcoord0 = vec2(0.0, 0.0);
	v_texcoord1 = vec2(0.0, 0.0);

#if defined(HAS_UV_SET1)
		v_texcoord0 = a_texcoord0;
#endif

#if defined(HAS_UV_SET2)
		v_texcoord1 = a_texcoord1;
#endif

#if defined(HAS_VERTEX_COLOR_VEC3) || defined(HAS_VERTEX_COLOR_VEC4)
		v_color = a_color0;
#endif

	gl_Position = u_ViewProjectionMatrix * pos;
}
