// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

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
#include <common/texture.hpp>
#include <common/controls.hpp>

const int ENEMY_NUMBER = 10;

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
    glfwSetCursorPos(window, 1024/2, 768/2);

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
	
	// Load the texture
	GLuint Texture = loadDDS("uvtemplate.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(FloorID, "myTextureSampler");

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
	 0.0f, 1.2f, 0.0f,
	-1.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 1.0f,

	 0.0f, 1.2f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	 1.0f, 0.0f,-1.0f,

	 0.0f, 1.2f, 0.0f,
	 1.0f, 0.0f,-1.0f,
	-1.0f, 0.0f,-1.0f,

	 0.0f, 1.2f, 0.0f,
	-1.0f, 0.0f,-1.0f,
	-1.0f, 0.0f, 1.0f,

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

	float enemy_coords[ENEMY_NUMBER][3];
	float enemy_radius = 10.0f;
	for (int i = 0; i < ENEMY_NUMBER; i++) {
		enemy_coords[i][0] = (float)std::rand() / RAND_MAX * 2 * enemy_radius - enemy_radius;
		enemy_coords[i][1] = (float)std::rand() / RAND_MAX * enemy_radius;
		enemy_coords[i][2] = (float)std::rand() / RAND_MAX * 2 * enemy_radius - enemy_radius;
		std::cout << enemy_coords[i][0] << " ";
	}
	float lastTime = glfwGetTime();
	do{

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
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 12*3 indices starting at 0 -> 12 triangles
		
		//ENEMY
		glUseProgram(EnemyID);

		for (int i = 0; i < ENEMY_NUMBER; i++) {
		  // Enemies appear eventually
			if (glfwGetTime() - lastTime < i)
				continue;

			ModelMatrix = glm::mat4(1.0);
			ModelMatrix[3][0] = enemy_coords[i][0];
			ModelMatrix[3][1] = enemy_coords[i][1];
			ModelMatrix[3][2] = enemy_coords[i][2];

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

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

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

