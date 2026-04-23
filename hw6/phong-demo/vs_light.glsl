#version 400

layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 vn;
out vec3 vert_pos, vert_norm;

void main () {
	vert_pos = vp;
	vert_norm =  vn;
	gl_Position = vec4(vert_pos, 1.0);
}