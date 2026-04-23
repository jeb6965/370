
//windows/mingw: g++ -o texture-demo.exe gl_utils.cpp math_funcs.cpp stb_image.h texture-demo.cpp -I include -lglew32 -lglfw3 -lgdi32 -lopengl32 -w

/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries separate legal notices                              |
|******************************************************************************|
| Phong Illumination                                                           |
| Apple: remember to uncomment version number hint in start_gl()               |
\******************************************************************************/


#include "math_funcs.h"
#include "gl_utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GL_LOG_FILE "gl.log"

// keep track of window size
int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow* g_window = NULL;

bool load_texture(const char* file_name, GLuint* tex) {
	int x, y, n;
	int force_channels = 4;

	// let stb_image do the vertical flip for OpenGL texture coordinates
	stbi_set_flip_vertically_on_load(true);

	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}

	// optional warning for NPOT textures
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name);
	}

	glGenTextures(1, tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// only use anisotropic filtering if supported
	if (GLEW_EXT_texture_filter_anisotropic) {
		GLfloat max_aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	}

	stbi_image_free(image_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

int main() {
	restart_gl_log();
	start_gl();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	// positions for 2 triangles forming a quad
	// cube vertices (36 vertices = 6 faces x 2 triangles x 3 verts)
    GLfloat points[] = {
        // front
         1, 1, 1,  -1, 1, 1,  -1,-1, 1,
        -1,-1, 1,   1,-1, 1,   1, 1, 1,
        // right
         1, 1, 1,   1,-1, 1,   1,-1,-1,
         1,-1,-1,   1, 1,-1,   1, 1, 1,
        // top
         1, 1, 1,   1, 1,-1,  -1, 1,-1,
        -1, 1,-1,  -1, 1, 1,   1, 1, 1,
        // left
        -1, 1, 1,  -1, 1,-1,  -1,-1,-1,
        -1,-1,-1,  -1,-1, 1,  -1, 1, 1,
        // bottom
        -1,-1,-1,   1,-1,-1,   1,-1, 1,
         1,-1, 1,  -1,-1, 1,  -1,-1,-1,
        // back
        -1, 1,-1,   1, 1,-1,   1,-1,-1,
         1,-1,-1,  -1,-1,-1,  -1, 1,-1
    };

	// normal array (36 normals)
	GLfloat normals[] = {
		// front
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		// right
		1, 0, 0, 1, 0, 0, 1, 0, 0,
		1, 0, 0, 1, 0, 0, 1, 0, 0,
		// top
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		// left
		-1, 0, 0, -1, 0, 0, -1, 0, 0,
		-1, 0, 0, -1, 0, 0, -1, 0, 0,
		// bottom
		0, -1, 0, 0, -1, 0, 0, -1, 0,
		0, -1, 0, 0, -1, 0, 0, -1, 0,
		// back
		0, 0, -1, 0, 0, -1, 0, 0, -1,
		0, 0, -1, 0, 0, -1, 0, 0, -1};
	// tex-coord array (36 vertices)
	GLfloat texcoords[] = {
		// front
		0, 1, 1, 1, 1, 0,
		1, 0, 0, 0, 0, 1,
		// righ
		1, 1, 1, 0, 0, 0,
		0, 0, 0, 1, 1, 1,
		// top
		0, 0, 0, 1, 1, 1,
		1, 1, 1, 0, 0, 0,
		// left
		0, 1, 1, 1, 1, 0,
		1, 0, 0, 0, 0, 1,
		// bottom
		1, 0, 0, 0, 0, 1,
		0, 1, 1, 1, 1, 0,
		// back
		0, 1, 1, 1, 1, 0,
		1, 0, 0, 0, 0, 1};
	int numPoints = 36;
	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint normals_vbo = 0;
	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	GLuint texcoords_vbo = 0;
	glGenBuffers(1, &texcoords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	GLuint shader_programme =
		create_programme_from_files("vs_texture.glsl", "fs_texture.glsl");

	#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0

	float near = 0.1f;
	float far = 100.0f;
	float fov = 67.0f * ONE_DEG_IN_RAD;
	float aspect = (float)g_gl_width / (float)g_gl_height;

	float range = tan(fov * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);

	GLfloat proj_mat[] = {
		Sx,   0.0f, 0.0f,  0.0f,
		0.0f, Sy,   0.0f,  0.0f,
		0.0f, 0.0f, Sz,   -1.0f,
		0.0f, 0.0f, Pz,    0.0f
	};

	float cam_pos[] = { 0.0f, 0.0f, 3.5f };
	float cam_yaw = 0.0f;

	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
	mat4 view_mat = R * T;

	mat4 model_mat = identity_mat4();

	GLuint tex = 0;
	assert(load_texture("rusty_metal_04_diff_4k.jpg", &tex));

	glUseProgram(shader_programme);

	int view_mat_location = glGetUniformLocation(shader_programme, "view_mat");
	int proj_mat_location = glGetUniformLocation(shader_programme, "projection_mat");
	int model_mat_location = glGetUniformLocation(shader_programme, "model_mat");
	int tex_location = glGetUniformLocation(shader_programme, "basic_texture");

	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
	glUniform1i(tex_location, 0); // use texture unit 0

	while (!glfwWindowShouldClose(g_window)) {
		_update_fps_counter(g_window);
		double current_seconds = glfwGetTime();

		glViewport(0, 0, g_gl_width, g_gl_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_programme);

		// simple left-right animation
		model_mat = identity_mat4();
		float angle = (float)current_seconds * 50.0f; // degrees per second
		model_mat = rotate_y_deg(identity_mat4(), angle);

		glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat.m);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numPoints);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}
		glfwSwapBuffers(g_window);
	}

	// Cleanup
	glDeleteTextures(1, &tex);
	glDeleteBuffers(1, &points_vbo);
	glDeleteBuffers(1, &normals_vbo);
	glDeleteBuffers(1, &texcoords_vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader_programme);
	glfwTerminate();
	return 0;
}