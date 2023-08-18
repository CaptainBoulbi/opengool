#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include <cstring>
#include <chrono>
#include <cmath>

#include "shader.hpp"

GLFWwindow* setup();
void renderLoop(GLFWwindow* window);
void tearDown();

unsigned int loadShader();

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(){
	GLFWwindow* window = setup();

	renderLoop(window);

	tearDown();
    return 0;
}

GLFWwindow* setup(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "8===D", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
		assert("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
		assert("Failed to initialize GLAD");
    }    

	// wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// activer par defaut, mode remplie
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	std::cout << glGetString(GL_VERSION) << std::endl;

	// min 16 vec4
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nb of vertex attributes supported: " << nrAttributes << std::endl;

	return window;
}

void processInput(GLFWwindow *window){
	// qwerty key ;(
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void setMode(int* mode){
	if (*mode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void switchMode(int* mode){
	*mode = !*mode;
	setMode(mode);
}

void clearErr(){
	std::cout << "All Error" << std::endl;
	unsigned int err = glGetError();
	while (err){
		std::cout << err << std::endl;
		err = glGetError();
	};
	std::cout << "Done with all err" << std::endl;
}

void getErr(){
	std::cout << glGetError() << std::endl;
}

void tearDown(){
    glfwTerminate();
}

void renderLoop(GLFWwindow* window){
	Shader shader1("shader/1.vs", "shader/1.fs");
	Shader shader2("shader/2.vs", "shader/2.fs");

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, 0.75f, 0.0f,
		 0.75f, 1.0f, 0.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		0, 1, 3,
		4, 5, 6
	};
	float vertices2[] = {
		-0.75f, -0.5f, 1.0f, 0.0f, 0.0f, // top
        -0.5f, -1.0f, 0.0f, 1.0f, 0.0f, // right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f  // corner 
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  

	unsigned int VBO2;
	glGenBuffers(1, &VBO2);
	unsigned int VAO2;
	glGenVertexArrays(1, &VAO2);

	glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::cout << std::endl;
	int mode = 0;
    for (int i=0; !glfwWindowShouldClose(window); i++){
		start = std::chrono::system_clock::now();
		processInput(window);

		if (!(i%20))
			switchMode(&mode);
		else
			setMode(&mode);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader1.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		float offset = (std::sin(glfwGetTime()) / 2.0f) + 0.5f;
		shader2.use();
		shader2.setFloat("offset", offset);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		if (!(i%5)){
			puts("\033[2F");
			std::cout << "fps : " << (int)(1 / elapsed_seconds.count()) << "      " << std::endl;
		}
    }

	clearErr();
	getErr();
}
