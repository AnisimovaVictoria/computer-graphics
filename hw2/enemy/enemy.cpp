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

// Create and compile our GLSL program from the shaders
GLuint programID = LoadShaders( "enemy.vertexshader", "enemy.fragmentshader" );

// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
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
static const GLfloat g_color_buffer_data[] = { 
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
GLuint vertexbuffer;
glGenBuffers(1, &vertexbuffer);
glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

GLuint colorbuffer;
glGenBuffers(1, &colorbuffer);
glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

void DisplayEnemy()
{
	/*// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around*/

	// Use our shader
	glUseProgram(programID);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 8*3); // 12*3 indices starting at 0 -> 12 triangles

	/*glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();*/
}


	/*// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();*/


