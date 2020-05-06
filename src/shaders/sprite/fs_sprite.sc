$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_texColor, 0);
uniform vec4 u_color0;

void main() {
	vec4 texCol = texture2D(s_texColor, v_texcoord0);
	vec4 color = mix(u_color0, texCol, texCol.w);
	if(color.w < 1.0/255.0) { discard; }
	gl_FragColor = color;
}