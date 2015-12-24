#version 450
layout(location = 0) out vec4 fragColor;

layout(location = 3) uniform float spriteAlpha;

void main() {
	vec2 v = gl_PointCoord * 2.0 - 1.0; // [0,1]^2 --> [0,2]^2 --> [-1,1]^2
	float alpha = spriteAlpha * (1.0 - clamp(dot(v,v), 0.0, 1.0)); // apply falloff

	fragColor = vec4(1.0, 1.0, 1.0, alpha);
}