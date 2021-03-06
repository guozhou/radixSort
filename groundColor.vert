#version 450
layout(location = 0) in vec2 pointIn;
layout(location = 1) in vec2 texCoordIn;

out vec2 texCoord;

layout(location = 0) uniform mat4 viewMat;
layout(location = 1) uniform mat4 projMat;

void main() {
	texCoord = texCoordIn;
	// clip coordinate (*, *, *, -eye.z), eye.z \in [-zNear, -zFar]
	gl_Position = projMat * viewMat * vec4(pointIn.x, -1.0, pointIn.y, 1.0);
}