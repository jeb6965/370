// Compile command with all arguments: g++ hello-open-gl.cpp -o hello -lX11 -lGL -lGLEW -lglfw
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <iostream>
int main() {
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	// uncomment these lines if on Apple OS X
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glPointSize(5.0f);
	glClearColor(0,0,0,1.0f);
	
	/* VERTEX POSITIONS */
	float positions[] = {
		// triangle 1
		0.0f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		// triangle 2
		-0.8f,  0.8f, 0.0f,
		-0.4f,  0.2f, 0.0f,
		-0.9f,  0.2f, 0.0f,
		// triangle 3
		0.4f,  0.8f, 0.0f,
		0.9f,  0.2f, 0.0f,
		0.3f,  0.2f, 0.0f,
	};

	/* VERTEX COLORS */
	float colors[] = {
		// triangle 1
		1.0f, 0.0f, 0.0f,  // red
		0.0f, 1.0f, 0.0f,  // green
		0.0f, 0.0f, 1.0f,  // blue
		// triangle 2
		1.0f, 1.0f, 0.0f,  // yellow
		1.0f, 0.0f, 1.0f,  // magenta
		0.0f, 1.0f, 1.0f,  // cyan
		// triangle 3
		1.0f, 0.5f, 0.0f,  // orange
		0.5f, 0.0f, 1.0f,  // purple
		0.0f, 1.0f, 0.5f,  // teal
	};

	// position VBO
	GLuint vbo_pos, vbo_col;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	// color VBO
	glGenBuffers(1, &vbo_col);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);     
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(1);               
	glBindBuffer(GL_ARRAY_BUFFER, vbo_col);    
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); 

	const char* vertex_shader =
    "#version 400\n"
    "layout(location = 0) in vec3 vp;"  // Vertex points
    "layout(location = 1) in vec3 color;"  // Vertex colors
    "out vec3 fragColor;"  // Sends to fragment shader
    "void main() {"
    "  gl_Position = vec4(vp.x, vp.y, vp.z, 1.0);"
    "  fragColor = color;"
    "}";

	const char* fragment_shader =
    "#version 400\n"
    "in vec3 fragColor;"  // Matches by name
    "out vec4 frag_color;"
    "void main() {"
    "  frag_color = vec4(fragColor, 1.0);"
    "}";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);


	int logLen;
	GLchar* log;
	glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &logLen);
	/*if(logLen > 0) {
	// Show any errors as appropriate
	glGetProgramInfoLog(shader_program, logLen, &logLen, log);
	fprintf(stderr, "Prog Info Log: %s\n", log);
	}*/
	while(!glfwWindowShouldClose(window)) {
		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glPointSize(5.0);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 9);
		// update other events like input handling
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
