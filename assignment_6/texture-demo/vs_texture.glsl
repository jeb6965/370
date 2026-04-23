#version 400

layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 vn;
layout (location = 2) in vec2 vt;

uniform mat4 projection_mat, view_mat, model_mat;

out vec3 vert_pos;
out vec3 vert_norm;
out vec2 tex_coord;

void main () {
    vert_pos = vec3(view_mat * model_mat * vec4(vp, 1.0));
    vert_norm = vec3(view_mat * model_mat * vec4(vn, 0.0));
    tex_coord = vt;

    gl_Position = projection_mat * vec4(vert_pos, 1.0);
}