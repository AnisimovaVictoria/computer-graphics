// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

const int ENEMY_NUMBER = 100;

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint FloorID = LoadShaders("TransformVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	GLuint FloorMatrixID = glGetUniformLocation(FloorID, "MVP");

	GLuint EnemyID = LoadShaders("EnemyTransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint EnemyMatrixID = glGetUniformLocation(EnemyID, "MVP");

	GLuint SphereID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	GLuint SphereMatrixID = glGetUniformLocation(SphereID, "MVP");

	// Load the texture
	// Get a handle for our "myTextureSampler" uniform
	GLuint SphereTexture = loadDDS("uvtemplate.DDS");
	GLuint SphereTextureID = glGetUniformLocation(SphereID, "myTextureSampler");

	//floor
	static const GLfloat g_vertex_buffer_data_floor[] = {
		-100.0f, 0.0f,-100.0f,
		-100.0f, 0.0f, 100.0f,
		 100.0f, 0.0f, 100.0f,

		 100.0f, 0.0f, 100.0f,
		 100.0f, 0.0f,-100.0f,
		-100.0f, 0.0f,-100.0f,
	};

	GLuint floorvertexbuffer;
	glGenBuffers(1, &floorvertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, floorvertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_floor), g_vertex_buffer_data_floor, GL_STATIC_DRAW);

	static const GLfloat g_vertex_buffer_data_enemy[] = {
	 0.0f, 2.0f, 0.0f,
	-1.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 1.0f,

	 0.0f, 2.0f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	 1.0f, 0.0f,-1.0f,

	 0.0f, 2.0f, 0.0f,
	 1.0f, 0.0f,-1.0f,
	-1.0f, 0.0f,-1.0f,

	 0.0f, 2.0f, 0.0f,
	-1.0f, 0.0f,-1.0f,
	-1.0f, 0.0f, 1.0f,

	 0.0f,-2.0f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 1.0f,

	 0.0f,-2.0f, 0.0f,
	 1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f, 1.0f,

	 0.0f,-2.0f, 0.0f,
	-1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f,-1.0f,

	 0.0f,-2.0f, 0.0f,
	-1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f,-1.0f,

	};

	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data_enemy[] = {
	0.583f,  0.771f,  0.9f,
	0.609f,  0.115f,  0.9f,
	0.327f,  0.483f,  0.844f,

	0.822f,  0.569f,  0.9f,
	0.435f,  0.602f,  0.9f,
	0.310f,  0.747f,  0.9f,

	0.597f,  0.770f,  0.761f,
	0.559f,  0.436f,  0.730f,
	0.359f,  0.583f,  0.9f,

	0.483f,  0.596f,  0.789f,
	0.559f,  0.861f,  0.639f,
	0.195f,  0.548f,  0.859f,

	0.014f,  0.184f,  0.576f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.9f,

	0.676f,  0.977f,  0.9f,
	0.971f,  0.572f,  0.833f,
	0.140f,  0.616f,  0.489f,

	0.997f,  0.513f,  0.9f,
	0.945f,  0.719f,  0.9f,
	0.543f,  0.021f,  0.978f,

	0.279f,  0.317f,  0.9f,
	0.167f,  0.620f,  0.9f,
	0.347f,  0.857f,  0.8f,

	};

	GLuint enemyvertexbuffer;
	glGenBuffers(1, &enemyvertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, enemyvertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_enemy), g_vertex_buffer_data_enemy, GL_STATIC_DRAW);

	GLuint enemycolorbuffer;
	glGenBuffers(1, &enemycolorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, enemycolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_enemy), g_color_buffer_data_enemy, GL_STATIC_DRAW);

	std::vector<std::vector<float>> enemy_coords;
	int enemy_count = 0;
	const float enemy_radius = 30.0f;

	// Our vertices. 
	static const GLfloat g_vertex_buffer_data_sphere[] = {
		-0.0000f, -0.0000f, 1.0000f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.1816f, -0.5590f, 0.8090f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.1816f, -0.5590f, 0.8090f,
		-0.0000f, -0.0000f, 1.0000f,
		0.1816f, -0.5590f, 0.8090f,
		0.0000f, -0.0000f, 1.0000f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.1816f, -0.5590f, 0.8090f,
		0.1816f, -0.5590f, 0.8090f,
		0.0000f, -0.0000f, 1.0000f,
		0.4755f, -0.3455f, 0.8090f,
		0.0000f, -0.0000f, 1.0000f,
		0.0000f, -0.0000f, 1.0000f,
		0.1816f, -0.5590f, 0.8090f,
		0.4755f, -0.3455f, 0.8090f,
		0.0000f, -0.0000f, 1.0000f,
		0.5878f, 0.0000f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		0.0000f, -0.0000f, 1.0000f,
		0.4755f, -0.3455f, 0.8090f,
		0.5878f, 0.0000f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		0.4755f, 0.3455f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		0.0000f, 0.0000f, 1.0000f,
		0.5878f, 0.0000f, 0.8090f,
		0.4755f, 0.3455f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		0.1816f, 0.5590f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		0.0000f, 0.0000f, 1.0000f,
		0.4755f, 0.3455f, 0.8090f,
		0.1816f, 0.5590f, 0.8090f,
		0.0000f, 0.0000f, 1.0000f,
		-0.1816f, 0.5590f, 0.8090f,
		-0.0000f, 0.0000f, 1.0000f,
		0.0000f, 0.0000f, 1.0000f,
		0.1816f, 0.5590f, 0.8090f,
		-0.1816f, 0.5590f, 0.8090f,
		-0.0000f, 0.0000f, 1.0000f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.0000f, 0.0000f, 1.0000f,
		-0.0000f, 0.0000f, 1.0000f,
		-0.1816f, 0.5590f, 0.8090f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.0000f, 0.0000f, 1.0000f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.0000f, -0.0000f, 1.0000f,
		-0.0000f, 0.0000f, 1.0000f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.2939f, -0.9045f, 0.3090f,
		-0.1816f, -0.5590f, 0.8090f,
		-0.4755f, -0.3455f, 0.8090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.2939f, -0.9045f, 0.3090f,
		-0.1816f, -0.5590f, 0.8090f,
		0.2939f, -0.9045f, 0.3090f,
		0.1816f, -0.5590f, 0.8090f,
		-0.1816f, -0.5590f, 0.8090f,
		-0.2939f, -0.9045f, 0.3090f,
		0.2939f, -0.9045f, 0.3090f,
		0.1816f, -0.5590f, 0.8090f,
		0.7694f, -0.5590f, 0.3090f,
		0.4755f, -0.3455f, 0.8090f,
		0.1816f, -0.5590f, 0.8090f,
		0.2939f, -0.9045f, 0.3090f,
		0.7694f, -0.5590f, 0.3090f,
		0.4755f, -0.3455f, 0.8090f,
		0.9511f, 0.0000f, 0.3090f,
		0.5878f, 0.0000f, 0.8090f,
		0.4755f, -0.3455f, 0.8090f,
		0.7694f, -0.5590f, 0.3090f,
		0.9511f, 0.0000f, 0.3090f,
		0.5878f, 0.0000f, 0.8090f,
		0.7694f, 0.5590f, 0.3090f,
		0.4755f, 0.3455f, 0.8090f,
		0.5878f, 0.0000f, 0.8090f,
		0.9511f, 0.0000f, 0.3090f,
		0.7694f, 0.5590f, 0.3090f,
		0.4755f, 0.3455f, 0.8090f,
		0.2939f, 0.9045f, 0.3090f,
		0.1816f, 0.5590f, 0.8090f,
		0.4755f, 0.3455f, 0.8090f,
		0.7694f, 0.5590f, 0.3090f,
		0.2939f, 0.9045f, 0.3090f,
		0.1816f, 0.5590f, 0.8090f,
		-0.2939f, 0.9045f, 0.3090f,
		-0.1816f, 0.5590f, 0.8090f,
		0.1816f, 0.5590f, 0.8090f,
		0.2939f, 0.9045f, 0.3090f,
		-0.2939f, 0.9045f, 0.3090f,
		-0.1816f, 0.5590f, 0.8090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.1816f, 0.5590f, 0.8090f,
		-0.2939f, 0.9045f, 0.3090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.5878f, -0.0000f, 0.8090f,
		-0.4755f, 0.3455f, 0.8090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.2939f, -0.9045f, -0.3090f,
		-0.2939f, -0.9045f, 0.3090f,
		-0.7694f, -0.5590f, 0.3090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.2939f, -0.9045f, -0.3090f,
		-0.2939f, -0.9045f, 0.3090f,
		0.2939f, -0.9045f, -0.3090f,
		0.2939f, -0.9045f, 0.3090f,
		-0.2939f, -0.9045f, 0.3090f,
		-0.2939f, -0.9045f, -0.3090f,
		0.2939f, -0.9045f, -0.3090f,
		0.2939f, -0.9045f, 0.3090f,
		0.7694f, -0.5590f, -0.3090f,
		0.7694f, -0.5590f, 0.3090f,
		0.2939f, -0.9045f, 0.3090f,
		0.2939f, -0.9045f, -0.3090f,
		0.7694f, -0.5590f, -0.3090f,
		0.7694f, -0.5590f, 0.3090f,
		0.9511f, 0.0000f, -0.3090f,
		0.9511f, 0.0000f, 0.3090f,
		0.7694f, -0.5590f, 0.3090f,
		0.7694f, -0.5590f, -0.3090f,
		0.9511f, 0.0000f, -0.3090f,
		0.9511f, 0.0000f, 0.3090f,
		0.7694f, 0.5590f, -0.3090f,
		0.7694f, 0.5590f, 0.3090f,
		0.9511f, 0.0000f, 0.3090f,
		0.9511f, 0.0000f, -0.3090f,
		0.7694f, 0.5590f, -0.3090f,
		0.7694f, 0.5590f, 0.3090f,
		0.2939f, 0.9045f, -0.3090f,
		0.2939f, 0.9045f, 0.3090f,
		0.7694f, 0.5590f, 0.3090f,
		0.7694f, 0.5590f, -0.3090f,
		0.2939f, 0.9045f, -0.3090f,
		0.2939f, 0.9045f, 0.3090f,
		-0.2939f, 0.9045f, -0.3090f,
		-0.2939f, 0.9045f, 0.3090f,
		0.2939f, 0.9045f, 0.3090f,
		0.2939f, 0.9045f, -0.3090f,
		-0.2939f, 0.9045f, -0.3090f,
		-0.2939f, 0.9045f, 0.3090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.2939f, 0.9045f, 0.3090f,
		-0.2939f, 0.9045f, -0.3090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.9511f, -0.0000f, 0.3090f,
		-0.7694f, 0.5590f, 0.3090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.4755f, -0.3455f, -0.8090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.5878f, -0.0000f, -0.8090f,
		-0.4755f, -0.3455f, -0.8090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.1816f, -0.5590f, -0.8090f,
		-0.2939f, -0.9045f, -0.3090f,
		-0.7694f, -0.5590f, -0.3090f,
		-0.4755f, -0.3455f, -0.8090f,
		-0.1816f, -0.5590f, -0.8090f,
		-0.2939f, -0.9045f, -0.3090f,
		0.1816f, -0.5590f, -0.8090f,
		0.2939f, -0.9045f, -0.3090f,
		-0.2939f, -0.9045f, -0.3090f,
		-0.1816f, -0.5590f, -0.8090f,
		0.1816f, -0.5590f, -0.8090f,
		0.2939f, -0.9045f, -0.3090f,
		0.4755f, -0.3455f, -0.8090f,
		0.7694f, -0.5590f, -0.3090f,
		0.2939f, -0.9045f, -0.3090f,
		0.1816f, -0.5590f, -0.8090f,
		0.4755f, -0.3455f, -0.8090f,
		0.7694f, -0.5590f, -0.3090f,
		0.5878f, 0.0000f, -0.8090f,
		0.9511f, 0.0000f, -0.3090f,
		0.7694f, -0.5590f, -0.3090f,
		0.4755f, -0.3455f, -0.8090f,
		0.5878f, 0.0000f, -0.8090f,
		0.9511f, 0.0000f, -0.3090f,
		0.4755f, 0.3455f, -0.8090f,
		0.7694f, 0.5590f, -0.3090f,
		0.9511f, 0.0000f, -0.3090f,
		0.5878f, 0.0000f, -0.8090f,
		0.4755f, 0.3455f, -0.8090f,
		0.7694f, 0.5590f, -0.3090f,
		0.1816f, 0.5590f, -0.8090f,
		0.2939f, 0.9045f, -0.3090f,
		0.7694f, 0.5590f, -0.3090f,
		0.4755f, 0.3455f, -0.8090f,
		0.1816f, 0.5590f, -0.8090f,
		0.2939f, 0.9045f, -0.3090f,
		-0.1816f, 0.5590f, -0.8090f,
		-0.2939f, 0.9045f, -0.3090f,
		0.2939f, 0.9045f, -0.3090f,
		0.1816f, 0.5590f, -0.8090f,
		-0.1816f, 0.5590f, -0.8090f,
		-0.2939f, 0.9045f, -0.3090f,
		-0.4755f, 0.3455f, -0.8090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.2939f, 0.9045f, -0.3090f,
		-0.1816f, 0.5590f, -0.8090f,
		-0.4755f, 0.3455f, -0.8090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.5878f, -0.0000f, -0.8090f,
		-0.9511f, -0.0000f, -0.3090f,
		-0.7694f, 0.5590f, -0.3090f,
		-0.4755f, 0.3455f, -0.8090f,
		-0.5878f, -0.0000f, -0.8090f,
		-0.5878f, -0.0000f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		-0.4755f, -0.3455f, -0.8090f,
		-0.5878f, -0.0000f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		0.0000f, 0.0000f, -1.0000f,
		-0.4755f, -0.3455f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		-0.1816f, -0.5590f, -0.8090f,
		-0.4755f, -0.3455f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		0.0000f, 0.0000f, -1.0000f,
		-0.1816f, -0.5590f, -0.8090f,
		-0.0000f, 0.0000f, -1.0000f,
		0.1816f, -0.5590f, -0.8090f,
		-0.1816f, -0.5590f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		-0.0000f, 0.0000f, -1.0000f,
		0.1816f, -0.5590f, -0.8090f,
		-0.0000f, 0.0000f, -1.0000f,
		0.4755f, -0.3455f, -0.8090f,
		0.1816f, -0.5590f, -0.8090f,
		-0.0000f, 0.0000f, -1.0000f,
		-0.0000f, 0.0000f, -1.0000f,
		0.4755f, -0.3455f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		0.5878f, 0.0000f, -0.8090f,
		0.4755f, -0.3455f, -0.8090f,
		-0.0000f, 0.0000f, -1.0000f,
		-0.0000f, -0.0000f, -1.0000f,
		0.5878f, 0.0000f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		0.4755f, 0.3455f, -0.8090f,
		0.5878f, 0.0000f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		-0.0000f, -0.0000f, -1.0000f,
		0.4755f, 0.3455f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		0.1816f, 0.5590f, -0.8090f,
		0.4755f, 0.3455f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		-0.0000f, -0.0000f, -1.0000f,
		0.1816f, 0.5590f, -0.8090f,
		0.0000f, -0.0000f, -1.0000f,
		-0.1816f, 0.5590f, -0.8090f,
		0.1816f, 0.5590f, -0.8090f,
		-0.0000f, -0.0000f, -1.0000f,
		0.0000f, -0.0000f, -1.0000f,
		-0.1816f, 0.5590f, -0.8090f,
		0.0000f, -0.0000f, -1.0000f,
		-0.4755f, 0.3455f, -0.8090f,
		-0.1816f, 0.5590f, -0.8090f,
		0.0000f, -0.0000f, -1.0000f,
		0.0000f, -0.0000f, -1.0000f,
		-0.4755f, 0.3455f, -0.8090f,
		0.0000f, 0.0000f, -1.0000f,
		-0.5878f, -0.0000f, -0.8090f,
		-0.4755f, 0.3455f, -0.8090f,
		0.0000f, -0.0000f, -1.0000f,
		0.0000f, 0.0000f, -1.0000f
	};

	// Two UV coordinatesfor each vertex. 
	static const GLfloat g_uv_buffer_data_sphere[] = {
		0.0000f, 1.0000f,
		0.1000f, 0.8000f,
		0.1000f, 1.0000f,
		0.0000f, 1.0000f,
		0.0000f, 0.8000f,
		0.1000f, 0.8000f,
		0.1000f, 1.0000f,
		0.2000f, 0.8000f,
		0.2000f, 1.0000f,
		0.1000f, 1.0000f,
		0.1000f, 0.8000f,
		0.2000f, 0.8000f,
		0.2000f, 1.0000f,
		0.3000f, 0.8000f,
		0.3000f, 1.0000f,
		0.2000f, 1.0000f,
		0.2000f, 0.8000f,
		0.3000f, 0.8000f,
		0.3000f, 1.0000f,
		0.4000f, 0.8000f,
		0.4000f, 1.0000f,
		0.3000f, 1.0000f,
		0.3000f, 0.8000f,
		0.4000f, 0.8000f,
		0.4000f, 1.0000f,
		0.5000f, 0.8000f,
		0.5000f, 1.0000f,
		0.4000f, 1.0000f,
		0.4000f, 0.8000f,
		0.5000f, 0.8000f,
		0.5000f, 1.0000f,
		0.6000f, 0.8000f,
		0.6000f, 1.0000f,
		0.5000f, 1.0000f,
		0.5000f, 0.8000f,
		0.6000f, 0.8000f,
		0.6000f, 1.0000f,
		0.7000f, 0.8000f,
		0.7000f, 1.0000f,
		0.6000f, 1.0000f,
		0.6000f, 0.8000f,
		0.7000f, 0.8000f,
		0.7000f, 1.0000f,
		0.8000f, 0.8000f,
		0.8000f, 1.0000f,
		0.7000f, 1.0000f,
		0.7000f, 0.8000f,
		0.8000f, 0.8000f,
		0.8000f, 1.0000f,
		0.9000f, 0.8000f,
		0.9000f, 1.0000f,
		0.8000f, 1.0000f,
		0.8000f, 0.8000f,
		0.9000f, 0.8000f,
		0.9000f, 1.0000f,
		1.0000f, 0.8000f,
		1.0000f, 1.0000f,
		0.9000f, 1.0000f,
		0.9000f, 0.8000f,
		1.0000f, 0.8000f,
		0.0000f, 0.8000f,
		0.1000f, 0.6000f,
		0.1000f, 0.8000f,
		0.0000f, 0.8000f,
		0.0000f, 0.6000f,
		0.1000f, 0.6000f,
		0.1000f, 0.8000f,
		0.2000f, 0.6000f,
		0.2000f, 0.8000f,
		0.1000f, 0.8000f,
		0.1000f, 0.6000f,
		0.2000f, 0.6000f,
		0.2000f, 0.8000f,
		0.3000f, 0.6000f,
		0.3000f, 0.8000f,
		0.2000f, 0.8000f,
		0.2000f, 0.6000f,
		0.3000f, 0.6000f,
		0.3000f, 0.8000f,
		0.4000f, 0.6000f,
		0.4000f, 0.8000f,
		0.3000f, 0.8000f,
		0.3000f, 0.6000f,
		0.4000f, 0.6000f,
		0.4000f, 0.8000f,
		0.5000f, 0.6000f,
		0.5000f, 0.8000f,
		0.4000f, 0.8000f,
		0.4000f, 0.6000f,
		0.5000f, 0.6000f,
		0.5000f, 0.8000f,
		0.6000f, 0.6000f,
		0.6000f, 0.8000f,
		0.5000f, 0.8000f,
		0.5000f, 0.6000f,
		0.6000f, 0.6000f,
		0.6000f, 0.8000f,
		0.7000f, 0.6000f,
		0.7000f, 0.8000f,
		0.6000f, 0.8000f,
		0.6000f, 0.6000f,
		0.7000f, 0.6000f,
		0.7000f, 0.8000f,
		0.8000f, 0.6000f,
		0.8000f, 0.8000f,
		0.7000f, 0.8000f,
		0.7000f, 0.6000f,
		0.8000f, 0.6000f,
		0.8000f, 0.8000f,
		0.9000f, 0.6000f,
		0.9000f, 0.8000f,
		0.8000f, 0.8000f,
		0.8000f, 0.6000f,
		0.9000f, 0.6000f,
		0.9000f, 0.8000f,
		1.0000f, 0.6000f,
		1.0000f, 0.8000f,
		0.9000f, 0.8000f,
		0.9000f, 0.6000f,
		1.0000f, 0.6000f,
		0.0000f, 0.6000f,
		0.1000f, 0.4000f,
		0.1000f, 0.6000f,
		0.0000f, 0.6000f,
		0.0000f, 0.4000f,
		0.1000f, 0.4000f,
		0.1000f, 0.6000f,
		0.2000f, 0.4000f,
		0.2000f, 0.6000f,
		0.1000f, 0.6000f,
		0.1000f, 0.4000f,
		0.2000f, 0.4000f,
		0.2000f, 0.6000f,
		0.3000f, 0.4000f,
		0.3000f, 0.6000f,
		0.2000f, 0.6000f,
		0.2000f, 0.4000f,
		0.3000f, 0.4000f,
		0.3000f, 0.6000f,
		0.4000f, 0.4000f,
		0.4000f, 0.6000f,
		0.3000f, 0.6000f,
		0.3000f, 0.4000f,
		0.4000f, 0.4000f,
		0.4000f, 0.6000f,
		0.5000f, 0.4000f,
		0.5000f, 0.6000f,
		0.4000f, 0.6000f,
		0.4000f, 0.4000f,
		0.5000f, 0.4000f,
		0.5000f, 0.6000f,
		0.6000f, 0.4000f,
		0.6000f, 0.6000f,
		0.5000f, 0.6000f,
		0.5000f, 0.4000f,
		0.6000f, 0.4000f,
		0.6000f, 0.6000f,
		0.7000f, 0.4000f,
		0.7000f, 0.6000f,
		0.6000f, 0.6000f,
		0.6000f, 0.4000f,
		0.7000f, 0.4000f,
		0.7000f, 0.6000f,
		0.8000f, 0.4000f,
		0.8000f, 0.6000f,
		0.7000f, 0.6000f,
		0.7000f, 0.4000f,
		0.8000f, 0.4000f,
		0.8000f, 0.6000f,
		0.9000f, 0.4000f,
		0.9000f, 0.6000f,
		0.8000f, 0.6000f,
		0.8000f, 0.4000f,
		0.9000f, 0.4000f,
		0.9000f, 0.6000f,
		1.0000f, 0.4000f,
		1.0000f, 0.6000f,
		0.9000f, 0.6000f,
		0.9000f, 0.4000f,
		1.0000f, 0.4000f,
		0.0000f, 0.4000f,
		0.1000f, 0.2000f,
		0.1000f, 0.4000f,
		0.0000f, 0.4000f,
		0.0000f, 0.2000f,
		0.1000f, 0.2000f,
		0.1000f, 0.4000f,
		0.2000f, 0.2000f,
		0.2000f, 0.4000f,
		0.1000f, 0.4000f,
		0.1000f, 0.2000f,
		0.2000f, 0.2000f,
		0.2000f, 0.4000f,
		0.3000f, 0.2000f,
		0.3000f, 0.4000f,
		0.2000f, 0.4000f,
		0.2000f, 0.2000f,
		0.3000f, 0.2000f,
		0.3000f, 0.4000f,
		0.4000f, 0.2000f,
		0.4000f, 0.4000f,
		0.3000f, 0.4000f,
		0.3000f, 0.2000f,
		0.4000f, 0.2000f,
		0.4000f, 0.4000f,
		0.5000f, 0.2000f,
		0.5000f, 0.4000f,
		0.4000f, 0.4000f,
		0.4000f, 0.2000f,
		0.5000f, 0.2000f,
		0.5000f, 0.4000f,
		0.6000f, 0.2000f,
		0.6000f, 0.4000f,
		0.5000f, 0.4000f,
		0.5000f, 0.2000f,
		0.6000f, 0.2000f,
		0.6000f, 0.4000f,
		0.7000f, 0.2000f,
		0.7000f, 0.4000f,
		0.6000f, 0.4000f,
		0.6000f, 0.2000f,
		0.7000f, 0.2000f,
		0.7000f, 0.4000f,
		0.8000f, 0.2000f,
		0.8000f, 0.4000f,
		0.7000f, 0.4000f,
		0.7000f, 0.2000f,
		0.8000f, 0.2000f,
		0.8000f, 0.4000f,
		0.9000f, 0.2000f,
		0.9000f, 0.4000f,
		0.8000f, 0.4000f,
		0.8000f, 0.2000f,
		0.9000f, 0.2000f,
		0.9000f, 0.4000f,
		1.0000f, 0.2000f,
		1.0000f, 0.4000f,
		0.9000f, 0.4000f,
		0.9000f, 0.2000f,
		1.0000f, 0.2000f,
		0.0000f, 0.2000f,
		0.1000f, 0.0000f,
		0.1000f, 0.2000f,
		0.0000f, 0.2000f,
		0.0000f, 0.0000f,
		0.1000f, 0.0000f,
		0.1000f, 0.2000f,
		0.2000f, 0.0000f,
		0.2000f, 0.2000f,
		0.1000f, 0.2000f,
		0.1000f, 0.0000f,
		0.2000f, 0.0000f,
		0.2000f, 0.2000f,
		0.3000f, 0.0000f,
		0.3000f, 0.2000f,
		0.2000f, 0.2000f,
		0.2000f, 0.0000f,
		0.3000f, 0.0000f,
		0.3000f, 0.2000f,
		0.4000f, 0.0000f,
		0.4000f, 0.2000f,
		0.3000f, 0.2000f,
		0.3000f, 0.0000f,
		0.4000f, 0.0000f,
		0.4000f, 0.2000f,
		0.5000f, 0.0000f,
		0.5000f, 0.2000f,
		0.4000f, 0.2000f,
		0.4000f, 0.0000f,
		0.5000f, 0.0000f,
		0.5000f, 0.2000f,
		0.6000f, 0.0000f,
		0.6000f, 0.2000f,
		0.5000f, 0.2000f,
		0.5000f, 0.0000f,
		0.6000f, 0.0000f,
		0.6000f, 0.2000f,
		0.7000f, 0.0000f,
		0.7000f, 0.2000f,
		0.6000f, 0.2000f,
		0.6000f, 0.0000f,
		0.7000f, 0.0000f,
		0.7000f, 0.2000f,
		0.8000f, 0.0000f,
		0.8000f, 0.2000f,
		0.7000f, 0.2000f,
		0.7000f, 0.0000f,
		0.8000f, 0.0000f,
		0.8000f, 0.2000f,
		0.9000f, 0.0000f,
		0.9000f, 0.2000f,
		0.8000f, 0.2000f,
		0.8000f, 0.0000f,
		0.9000f, 0.0000f,
		0.9000f, 0.2000f,
		1.0000f, 0.0000f,
		1.0000f, 0.2000f,
		0.9000f, 0.2000f,
		0.9000f, 0.0000f,
		1.0000f, 0.0000f
	};

	GLuint spherevertexbuffer;
	glGenBuffers(1, &spherevertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, spherevertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_sphere), g_vertex_buffer_data_sphere, GL_STATIC_DRAW);

	GLuint sphereuvbuffer;
	glGenBuffers(1, &sphereuvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereuvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data_sphere), g_uv_buffer_data_sphere, GL_STATIC_DRAW);

	std::vector<std::vector<glm::vec3>> sphere_coords; // ïîëîæåíèå è íàïðàâëåíèå
	float sphere_speed = 15.0f;

	const float beginTime = glfwGetTime();

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		//FLOOR
		// Use our shader
		glUseProgram(FloorID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(FloorMatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, floorvertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3); // 12*3 indices starting at 0 -> 12 triangles
    
		//TIME
		double currentTime = glfwGetTime();
		static double lastTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		
		//ENEMY
		glUseProgram(EnemyID);
		
		
		if (currentTime - beginTime > enemy_count && enemy_count < ENEMY_NUMBER) {
			std::vector<float> new_enemy = std::vector<float>({
				(float)std::rand() / RAND_MAX * 2 * enemy_radius - enemy_radius + getPosition()[0],
				(float)std::rand() / RAND_MAX * enemy_radius + 2.0f,
				(float)std::rand() / RAND_MAX * 2 * enemy_radius - enemy_radius + getPosition()[2],
				(float)std::rand() / RAND_MAX,
				(float)std::rand() / RAND_MAX,
				(float)std::rand() / RAND_MAX,
				((float)std::rand() / RAND_MAX) * 360.0f,
			});
			enemy_coords.push_back(new_enemy);
			enemy_count++;
		}
	  
		for (int i = 0; i < enemy_coords.size(); i++) {
			ModelMatrix = glm::mat4(1.0);
			
			glm::vec3 myRotationAxis(enemy_coords[i][3], enemy_coords[i][4], enemy_coords[i][5]);
			ModelMatrix = glm::rotate(enemy_coords[i][6], myRotationAxis) * ModelMatrix;
			
			glm::vec3 myTranslationVector(enemy_coords[i][0], enemy_coords[i][1], enemy_coords[i][2]);
			ModelMatrix = glm::translate(glm::mat4(), myTranslationVector) * ModelMatrix;

			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(EnemyMatrixID, 1, GL_FALSE, &MVP[0][0]);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, enemyvertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, enemycolorbuffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 8 * 3);
		}
		
		// SPHERE

		//Create sphere if mouse was released
		static int oldState = GLFW_RELEASE;
		int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
			std::vector<glm::vec3> new_sphere = std::vector<glm::vec3>({
				getPosition() + getDirection()*5.0f,
				getDirection()
				});
			sphere_coords.push_back(new_sphere);
		}
		oldState = newState;

		//Draw spheres
		for (int i = 0; i < sphere_coords.size(); i++) {
			// Use our shader
			glUseProgram(SphereID);

			ModelMatrix = glm::mat4(0.7f);

			glm::vec3 myTranslationVector(sphere_coords[i][0]);
			ModelMatrix = glm::translate(glm::mat4(), myTranslationVector) * ModelMatrix;

			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(SphereMatrixID, 1, GL_FALSE, &MVP[0][0]);

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SphereTexture);
			// Set our "myTextureSampler" sampler to use Texture Unit 0
			glUniform1i(SphereTextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, spherevertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, sphereuvbuffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size : U+V => 2
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 100 * 3); // 12*3 indices starting at 0 -> 12 triangles
																							
			//Update sphere coords
			sphere_coords[i][0] += sphere_coords[i][1] * sphere_speed * deltaTime;
		}

		//Collider
		for (int i = 0; i < enemy_coords.size(); i++) {
			glm::vec3 enemy_pos = glm::vec3(enemy_coords[i][0], enemy_coords[i][1], enemy_coords[i][2]);
			for (int j = 0; j < sphere_coords.size(); j++) {
				if (glm::distance(enemy_pos, sphere_coords[j][0]) < 0.7+1.2) {
					enemy_coords.erase(enemy_coords.begin()+i);
					sphere_coords.erase(sphere_coords.begin() + j);
				}
			}
		}

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;
		
		// Save if S is released
		//Create sphere if mouse was released
		static int oldStateS = GLFW_RELEASE;
		int newStateS = glfwGetKey(window, GLFW_KEY_S);
		if (newStateS == GLFW_RELEASE && oldStateS == GLFW_PRESS) {
			std::ofstream file;
			file.open("save.txt", std::ios::out | std::ios::trunc);
			glm::vec3 position = getPosition();
			file << position[0] << " " << position[1] << " " << position[2] << " "
				<< getHorizontalAngle() << " " << getVerticalAngle() << std::endl;

			file << enemy_count << std::endl; // enemy_count = dead + living enemies
			
			file << enemy_coords.size() << std::endl;
			for (int i = 0; i < enemy_coords.size(); i++)
				for (int j = 0; j < 7; j++)
					file << enemy_coords[i][j] << std::endl;

			file << sphere_coords.size() << std::endl;
			for (int i = 0; i < sphere_coords.size(); i++) {
				file << sphere_coords[i][0][0] << std::endl;
				file << sphere_coords[i][0][1] << std::endl;
				file << sphere_coords[i][0][2] << std::endl;
				file << sphere_coords[i][1][0] << std::endl;
				file << sphere_coords[i][1][1] << std::endl;
				file << sphere_coords[i][1][2] << std::endl;
			}

			file.close();
			std::cout << "ok" << std::endl;
		}
		oldStateS = newStateS;
		
		// Download if D is released
		static int oldStateD = GLFW_RELEASE;
		int newStateD = glfwGetKey(window, GLFW_KEY_D);
		if (newStateD == GLFW_RELEASE && oldStateD == GLFW_PRESS) {
			std::ifstream file;
			file.open("save.txt", std::ios::in);
			float x, y, z, a, b;
			file >> x >> y >> z >> a >> b;
			setPositionAndDirection(glm::vec3(x, y, z), a, b);

			file >> enemy_count; // enemy_count = dead + living enemies

			enemy_coords.clear();

			int n; // n = living enemies 
			file >> n;

			for (int i = 0; i < n; i++) {
				std::vector<float> v;
				for (int j = 0; j < 7; j++) {
					float x;
					file >> x;
					v.push_back(x);
				}
				enemy_coords.push_back(v);
			}

			sphere_coords.clear();
			file >> n;
			for (int i = 0; i < n; i++) {
				float a, b, c, d, e, f;
				file >> a >> b >> c >> d >> e >> f;
				sphere_coords.push_back(std::vector<glm::vec3>({ glm::vec3(a, b, c), glm::vec3(d, e, f) }));
			}
			file.close();
			std::cout << "ok" << std::endl;
		}
		oldStateD = newStateD;

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &floorvertexbuffer);

	//glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(FloorID);
	//glDeleteProgram(programID);
	//glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
