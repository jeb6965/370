// run g++ -o phong-demo.exe gl_utils.cpp phong-demo.cpp -I include -lglew32 -lglfw3 -lgdi32 -lopengl32 -w
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include "gl_utils.h"

int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow* g_window = NULL;

// Count how many lines in the OBJ begin with a given label, such as "v" or "f"
int countLabel(const std::string& modelName, const char label[]) {
	FILE* objFile = fopen(modelName.c_str(), "r");
	if (!objFile) {
		std::cerr << "ERROR: could not open model file: " << modelName << "\n";
		return 0;
	}

	int numLab = 0;
	char buf[128];

	while (fscanf(objFile, "%127s", buf) != EOF) {
		if (strcmp(buf, label) == 0) {
			numLab++;
		}
	}

	std::cout << "Model has " << numLab << " " << label << "\n";
	fclose(objFile);
	return numLab;
}

// Load vertex positions, center the model, scale it uniformly, and apply a small Y rotation
void loadVertices(const std::string& modelName, GLfloat verts[]) {
	std::cout << "Loading vertices\n";

	FILE* objFile = fopen(modelName.c_str(), "r");
	if (!objFile) {
		std::cerr << "ERROR: could not open model file: " << modelName << "\n";
		return;
	}

	int numVert = 0;

	float maxX = -1e8f, minX = 1e8f;
	float maxY = -1e8f, minY = 1e8f;
	float maxZ = -1e8f, minZ = 1e8f;

	char buf[128];
	float a, b, c;

	while (fscanf(objFile, "%127s", buf) != EOF) {
		if (strcmp(buf, "v") == 0) {
			if (fscanf(objFile, "%f %f %f", &a, &b, &c) != 3) {
				std::cerr << "WARNING: malformed vertex line in OBJ file.\n";
				continue;
			}

			if (a > maxX) maxX = a;
			if (a < minX) minX = a;
			if (b > maxY) maxY = b;
			if (b < minY) minY = b;
			if (c > maxZ) maxZ = c;
			if (c < minZ) minZ = c;

			verts[3 * numVert + 0] = a;
			verts[3 * numVert + 1] = b;
			verts[3 * numVert + 2] = c;
			numVert++;
		}
	}

	fclose(objFile);

	float scaleX = maxX - minX;
	float scaleY = maxY - minY;
	float scaleZ = maxZ - minZ;

	float transX = 0.5f * (maxX + minX);
	float transY = 0.5f * (maxY + minY);
	float transZ = 0.5f * (maxZ + minZ);

	// Uniform scale preserves the model proportions
	float scale = std::max(scaleX, std::max(scaleY, scaleZ));
	if (scale == 0.0f) scale = 1.0f;

	std::cout << "scales: " << scaleX << ", " << scaleY << ", " << scaleZ << "\n";
	std::cout << "center: " << transX << ", " << transY << ", " << transZ << "\n";

	for (int i = 0; i < numVert; i++) {
		// Center and scale
		verts[3 * i + 0] = (verts[3 * i + 0] - transX) / scale;
		verts[3 * i + 1] =  (verts[3 * i + 1] - transY) / scale;
		verts[3 * i + 2] = (verts[3 * i + 2] - transZ) / scale;
	}

	std::cout << "Done loading vertices\n";
}

// Load triangle faces in the form: f v1 v2 v3
// This assumes the OBJ has already been triangulated and uses plain integer indices
void loadFaces(const std::string& modelName, GLint faces[]) {
	std::cout << "Loading faces\n";

	FILE* objFile = fopen(modelName.c_str(), "r");
	if (!objFile) {
		std::cerr << "ERROR: could not open model file: " << modelName << "\n";
		return;
	}

	int numFaces = 0;
	char line[256];

	while (fgets(line, sizeof(line), objFile)) {
		if (line[0] == 'f' && line[1] == ' ') {
			int v1, v2, v3;
			int t1, t2, t3;
			int n1, n2, n3;

			// Case 1: f v/vt/vn v/vt/vn v/vt/vn
			if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&v1, &t1, &n1,
				&v2, &t2, &n2,
				&v3, &t3, &n3) == 9) {

				faces[3 * numFaces + 0] = v1 - 1;
				faces[3 * numFaces + 1] = v2 - 1;
				faces[3 * numFaces + 2] = v3 - 1;
				numFaces++;
			}
			// Case 2: f v//vn v//vn v//vn
			else if (sscanf(line, "f %d//%d %d//%d %d//%d",
				&v1, &n1,
				&v2, &n2,
				&v3, &n3) == 6) {

				faces[3 * numFaces + 0] = v1 - 1;
				faces[3 * numFaces + 1] = v2 - 1;
				faces[3 * numFaces + 2] = v3 - 1;
				numFaces++;
			}
			// Case 3: f v/vt v/vt v/vt
			else if (sscanf(line, "f %d/%d %d/%d %d/%d",
				&v1, &t1,
				&v2, &t2,
				&v3, &t3) == 6) {

				faces[3 * numFaces + 0] = v1 - 1;
				faces[3 * numFaces + 1] = v2 - 1;
				faces[3 * numFaces + 2] = v3 - 1;
				numFaces++;
			}
			// Case 4: f v v v
			else if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3) {
				faces[3 * numFaces + 0] = v1 - 1;
				faces[3 * numFaces + 1] = v2 - 1;
				faces[3 * numFaces + 2] = v3 - 1;
				numFaces++;
			}
			else {
				std::cerr << "WARNING: unsupported face format: " << line;
			}
		}
	}

	fclose(objFile);
	std::cout << "Done loading faces\n";
}

// Compute one normal per triangle face using the cross product of two triangle edges
void computeFaceNormals(GLfloat faceNormals[], const GLfloat verts[], const GLint faces[], int numFaces) {
	for (int i = 0; i < numFaces; i++) {
		int idx1 = faces[i * 3 + 0];
		int idx2 = faces[i * 3 + 1];
		int idx3 = faces[i * 3 + 2];

		// Edge vectors from vertex 1
		float ux = verts[idx2 * 3 + 0] - verts[idx1 * 3 + 0];
		float uy = verts[idx2 * 3 + 1] - verts[idx1 * 3 + 1];
		float uz = verts[idx2 * 3 + 2] - verts[idx1 * 3 + 2];

		float vx = verts[idx3 * 3 + 0] - verts[idx1 * 3 + 0];
		float vy = verts[idx3 * 3 + 1] - verts[idx1 * 3 + 1];
		float vz = verts[idx3 * 3 + 2] - verts[idx1 * 3 + 2];

		// Cross product u x v
		float nx = uy * vz - uz * vy;
		float ny = uz * vx - ux * vz;
		float nz = ux * vy - uy * vx;

		float mag = sqrtf(nx * nx + ny * ny + nz * nz);
		if (mag == 0.0f) mag = 1.0f;

		faceNormals[3 * i + 0] = nx / mag;
		faceNormals[3 * i + 1] = ny / mag;
		faceNormals[3 * i + 2] = nz / mag;
	}
}

// Compute one normal per vertex by averaging normals of adjacent faces
void computeVertNormals(GLfloat normals[], int numVerts, const GLint faces[], int numFaces, const GLfloat faceNormals[]) {
	// Zero-initialize
	for (int i = 0; i < numVerts * 3; i++) normals[i] = 0.0f;

	// One pass over faces: accumulate face normals into each vertex
	for (int j = 0; j < numFaces; j++) {
		for (int k = 0; k < 3; k++) {
			int idx = faces[j * 3 + k];
			normals[idx * 3 + 0] += faceNormals[j * 3 + 0];
			normals[idx * 3 + 1] += faceNormals[j * 3 + 1];
			normals[idx * 3 + 2] += faceNormals[j * 3 + 2];
		}
	}

	// One pass over vertices: normalize
	for (int i = 0; i < numVerts; i++) {
		float nx = normals[i * 3 + 0];
		float ny = normals[i * 3 + 1];
		float nz = normals[i * 3 + 2];
		float mag = sqrtf(nx*nx + ny*ny + nz*nz);
		if (mag == 0.0f) mag = 1.0f;
		normals[i * 3 + 0] = nx / mag;
		normals[i * 3 + 1] = ny / mag;
		normals[i * 3 + 2] = nz / mag;
	}
}

int main() {
	std::string modelName = "dragon.obj";

	int numVert = countLabel(modelName, "v");
	if (numVert <= 0) {
		std::cerr << "ERROR: no vertices found.\n";
		return 1;
	}

	GLfloat* verts = new GLfloat[3 * numVert];
	loadVertices(modelName, verts);

	int numFaces = countLabel(modelName, "f");
	if (numFaces <= 0) {
		std::cerr << "ERROR: no faces found.\n";
		return 1;
	}

	GLint* faces = new GLint[3 * numFaces];
	loadFaces(modelName, faces);

	GLfloat* faceNormals = new GLfloat[3 * numFaces];
	computeFaceNormals(faceNormals, verts, faces, numFaces);

	GLfloat* vertNormals = new GLfloat[3 * numVert];
	computeVertNormals(vertNormals, numVert, faces, numFaces, faceNormals);

	// Convert from indexed mesh to a flat triangle list for drawing with glDrawArrays
	GLfloat* points = new GLfloat[9 * numFaces];
	GLfloat* normals = new GLfloat[9 * numFaces];

	for (int i = 0; i < numFaces; i++) {
		int idx1 = faces[3 * i + 0];
		int idx2 = faces[3 * i + 1];
		int idx3 = faces[3 * i + 2];

		// Triangle positions
		points[i * 9 + 0] = verts[3 * idx1 + 0];
		points[i * 9 + 1] = verts[3 * idx1 + 1];
		points[i * 9 + 2] = verts[3 * idx1 + 2];

		points[i * 9 + 3] = verts[3 * idx2 + 0];
		points[i * 9 + 4] = verts[3 * idx2 + 1];
		points[i * 9 + 5] = verts[3 * idx2 + 2];

		points[i * 9 + 6] = verts[3 * idx3 + 0];
		points[i * 9 + 7] = verts[3 * idx3 + 1];
		points[i * 9 + 8] = verts[3 * idx3 + 2];

		// Triangle normals
		normals[i * 9 + 0] = vertNormals[3 * idx1 + 0];
		normals[i * 9 + 1] = vertNormals[3 * idx1 + 1];
		normals[i * 9 + 2] = vertNormals[3 * idx1 + 2];

		normals[i * 9 + 3] = vertNormals[3 * idx2 + 0];
		normals[i * 9 + 4] = vertNormals[3 * idx2 + 1];
		normals[i * 9 + 5] = vertNormals[3 * idx2 + 2];

		normals[i * 9 + 6] = vertNormals[3 * idx3 + 0];
		normals[i * 9 + 7] = vertNormals[3 * idx3 + 1];
		normals[i * 9 + 8] = vertNormals[3 * idx3 + 2];
	}

	int numPoints = 3 * numFaces;

	// Initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	g_window = glfwCreateWindow(g_gl_width, g_gl_height, "Phong Demo", NULL, NULL);
	glfwSetWindowPos(g_window, 500, 100);

	if (!g_window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(g_window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// OpenGL state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Buffer objects
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint normals_vbo = 0;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * numPoints * sizeof(GLfloat), points, GL_STATIC_DRAW);

	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * numPoints * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Load shaders
	//GLuint shader_programme = create_programme_from_files("vs_light.glsl", "fs_phong.glsl");
	//GLuint shader_programme = create_programme_from_files("vs_light.glsl", "fs-distance.glsl");
	//GLuint shader_programme = create_programme_from_files("vs_light.glsl", "fs-spotlight.glsl");
	GLuint shader_programme = create_programme_from_files("vs_light.glsl", "fs-toon.glsl");
	// Render loop
	while (!glfwWindowShouldClose(g_window)) {
		glViewport(0, 0, g_gl_width, g_gl_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_programme);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numPoints);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}

		glfwSwapBuffers(g_window);
	}

	// Cleanup
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &normals_vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader_programme);
	glfwTerminate();
	return 0;
}