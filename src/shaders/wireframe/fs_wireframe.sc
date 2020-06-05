$input v_ptColor

#include "../common/bgfx_shader.sh"

uniform vec4 u_thickness;

void main() {
	vec3 color = v_ptColor.xyz;
	float opacity = v_ptColor.w;
	float thick = u_thickness.x;

	if (gl_FrontFacing) {
		opacity *= 0.5;
	}

	vec3 fw = abs(dFdx(color)) + abs(dFdy(color));
	vec3 val =
		smoothstep(vec3_splat(0.0), fw * thick, color);
	float edge = min(min(val.x, val.y), val.z); 

	vec4 rgba = vec4(color, (1.0 - edge) * opacity);
	gl_FragColor = rgba;
}
