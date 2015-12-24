#version 450
in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

vec4 chessboard(ivec2 xy)
{
	ivec2 c = xy / 32; // stride
	c = c % 2;
	if(c.x == c.y)
		return vec4(0.85, 0.85, 1.0, 1.0);
	else
		return vec4(142.0/255.0, 142.0/255.0, 147.0/255.0, 1.0);
}

void main() {
	fragColor = chessboard(ivec2(texCoord * 512.0));
}