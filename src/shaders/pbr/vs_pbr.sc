$input a_position, a_texcoord0, a_normal, a_tangent
$output v_position, v_uv, v_normal, v_tbn0, v_tbn1, v_tbn2

// AUTHOR: KhronosGroup/glTF-Sample-Viewer
// LICENSE: APACHE 2

#include "bgfx_shader.sh"
#include "defines.sh"

void main()
{
	mat4 modelMtx = u_model[0];
	vec4 pos = modelMtx * a_position;
	v_position = vec3(pos.xyz) / pos.w;

	if(u_hasNormals > 0.0) { // HAS NORMALS
		if(u_hasTangents > 0.0) { // HAS TANGENTS
			vec3 normalW = normalize(
				vec3(modelMtx * vec4(a_normal.xyz, 0.0)));
			vec3 tangentW = normalize(
				vec3(modelMtx * vec4(a_tangent.xyz, 0.0)));
			vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
			mat3 v_tbn = mat3(tangentW, bitangentW, normalW);
			v_tbn0 = v_tbn[0];
			v_tbn1 = v_tbn[1];
			v_tbn2 = v_tbn[2];
		} else {
			v_normal = normalize(
				vec3(modelMtx * vec4(a_normal.xyz, 0.0)));
		}
	}

	if(u_hasUV > 0.0) { // HAS UV
		v_uv = a_texcoord0;
	} else {
		v_uv = vec2(0.,0.);
	}

	// needs w for proper perspective correction
	gl_Position = u_modelViewProj * a_position; 
}