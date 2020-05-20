$input a_position, a_color0
$output v_ptColor

#include <bgfx_shader.sh>

void main() {
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

	v_ptColor = a_color0;
}
