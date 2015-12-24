#version 450
layout(location = 0) in vec4 pointIn;

layout(location = 0) uniform mat4 viewMat;
layout(location = 1) uniform mat4 projMat;
layout(location = 2) uniform float radiusClip; // sprite radius in clip coordinate

void main() {
	// clip coordinate (*, *, *, -eye.z), eye.z \in [-zNear, -zFar]
	gl_Position = projMat * viewMat * pointIn;
	gl_PointSize = radiusClip / gl_Position.w;
}