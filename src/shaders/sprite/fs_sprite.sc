$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_texColor, 0);
uniform vec4 u_color0;
uniform vec4 u_opacity;

void main() {
	vec4 texCol = texture2D(s_texColor, v_texcoord0);
	vec3 colorRGB = mix(
		u_color0.xyz, texCol.xyz * u_opacity.xyz,
		abs(-1.0 + u_color0.w));
	vec4 colorRGBA = vec4(colorRGB, texCol.w * u_opacity.w);
	if (colorRGBA.w < 1.0 / 255.0) discard;
	gl_FragColor = colorRGBA;
}