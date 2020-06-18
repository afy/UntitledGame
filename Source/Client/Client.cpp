#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "glew-2.1.0/include/GL/glew.h"
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "FastNoiseSimd/FastNoiseSIMD/FastNoiseSIMD.h"

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

int main() 
{
	//Base setup
	sf::RenderWindow window(sf::VideoMode(800, 600), "Untitled Game");
	sf::Event ev;
	sf::Clock clock;
	sf::ContextSettings cs = window.getSettings();
	std::cout << "using OPENGL version: " << cs.majorVersion << "." << cs.minorVersion << "\n";
	std::cout << "using GLEW version: " << glewGetString(GLEW_VERSION) << "\n";
	bool running = true;

	//GLEW setup
	if (glewInit() != GLEW_OK) 
	{
		return -1;
	}

	//GL setup
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Shaders
	GLuint shader1 = LoadShaders("./Shaders/Test.vert", "./Shaders/Test.frag");

	// VAOs and objects 

	
	FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
	float* noiseSet = myNoise->GetSimplexFractalSet(0, 0, 0, 16, 16, 16);
	int index = 0;

	static float obj1[16 * 16 * 3];
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++) 
		{
			obj1[index] = (float)x;
			obj1[index + 1] = (float)noiseSet[index];
			obj1[index + 2] = (float)z;
			index += 3;
		}
	}
	int vertsLength = 16 * 16 * 3;
	

	/*
	static const float obj1[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};
	int vertsLength = 3;
	*/

	GLuint vbuff;
	glGenBuffers(1, &vbuff);
	glBindBuffer(GL_ARRAY_BUFFER, vbuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj1), obj1, GL_STATIC_DRAW);

	// Loop
	while (running) 
	{
		while (window.pollEvent(ev)) 
		{
			if (ev.type == sf::Event::Closed)
			{
				running = false;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader1);
		int viewLoc = glGetUniformLocation(shader1, "view");
		int projLoc = glGetUniformLocation(shader1, "projection");
		int modelLoc = glGetUniformLocation(shader1, "model");

		int radius = 10.0f;
		int time = 0; // clock.getElapsedTime().asSeconds();
		float camX = sin(time) * radius;
		float camZ = cos(time) * radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(8, 0, 8), glm::vec3(0, 1, 0));
		glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
		glm::mat4 model = glm::mat4(1.0f);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbuff);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glPointSize(3);
		glDrawArrays(GL_POINTS, 0, vertsLength);	
		glDisableVertexAttribArray(0);

		window.display();
	}

	window.close();

	// FREE UP FUCKING RRESOURCES B4 MEMORY LEAKS TAKE OVER DIPSHIT

	return 0;
}