// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

static const GLfloat g_vertex_buffer_data[] = { 
	 0.0f, 1.2f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 1.0f,

	 0.0f, 1.2f, 0.0f,
	 1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f, 1.0f,

	 0.0f, 1.2f, 0.0f,
	-1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f,-1.0f,

	 0.0f, 1.2f, 0.0f,
	-1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f,-1.0f,
	 
	 0.0f,-1.2f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 1.0f,

	 0.0f,-1.2f, 0.0f,
	 1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f, 1.0f,

	 0.0f,-1.2f, 0.0f,
	-1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f,-1.0f,

	 0.0f,-1.2f, 0.0f,
	-1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f,-1.0f,

	};

// One color for each vertex. They were generated randomly.
static const GLfloat enemy_colors[] = { 
	0.583f,  0.771f,  0.9f, 0.9f,
	0.609f,  0.115f,  0.9f, 0.9f,
	0.327f,  0.483f,  0.844f,0.9f,

	0.822f,  0.569f,  0.9f,0.9f,
	0.435f,  0.602f,  0.9f,0.9f,
	0.310f,  0.747f,  0.9f,0.9f,

	0.597f,  0.770f,  0.761f,0.9f,
	0.559f,  0.436f,  0.730f,0.9f,
	0.359f,  0.583f,  0.9f,0.9f,

	0.483f,  0.596f,  0.789f,0.9f,
	0.559f,  0.861f,  0.639f,0.9f,
	0.195f,  0.548f,  0.859f,0.9f,

	0.014f,  0.184f,  0.576f,0.9f,
	0.771f,  0.328f,  0.970f,0.9f,
	0.406f,  0.615f,  0.9f,0.9f,

	0.676f,  0.977f,  0.9f,0.9f,
	0.971f,  0.572f,  0.833f,0.9f,
	0.140f,  0.616f,  0.489f,0.9f,

	0.997f,  0.513f,  0.9f,0.9f,
	0.945f,  0.719f,  0.9f,0.9f,
	0.543f,  0.021f,  0.978f,0.9f,

	0.279f,  0.317f,  0.9f,0.9f,
	0.167f,  0.620f,  0.9f,0.9f,
	0.347f,  0.857f,  0.8f,0.9f,
};
