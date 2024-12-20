#define RGL_LOAD_IMPLEMENTATION
#include "rglLoad.h"

#define RGFW_ALLOC_DROPFILES
#define RGFW_IMPLEMENTATION
#define RGFW_PRINT_ERRORS
#include <RGFW.h>

// #include "glad/glad.h"
// #include "GLFW/glfw3.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <cassert>
#include <cstring>
#include <chrono>
#include <cmath>

#include "shader.hpp"

RGFW_window* setup();
void renderLoop(RGFW_window* window);
void tearDown();

unsigned int loadShader();

void processEvent(RGFW_window *window);
void framebuffer_size_callback(RGFW_window* window, int width, int height);
void mouse_callback(RGFW_window* window, double xpos, double ypos);

//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2;
float sensitivity = 0.1f;

float FOV = 45.0f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float timeLastFrame = 0.0f;
float currentTime = 0.0f;
u64 beginTime = 0.0f;

int main(){
	RGFW_window* window = setup();

	renderLoop(window);

	tearDown();
	return 0;
}

RGFW_window* setup(){
	// glfwInit();
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	RGFW_setGLVersion(RGFW_GL_CORE, 3, 3);

#ifdef __APPLE__
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	RGFW_window* window = RGFW_createWindow("8===3,", RGFW_RECT(SCR_WIDTH, SCR_HEIGHT, SCR_WIDTH, SCR_HEIGHT), RGFW_HIDE_MOUSE);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		// glfwTerminate();
		assert("Failed to create GLFW window");
	}
    RGFW_window_makeCurrent(window);
	// glfwSwapInterval(0);
	// glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);

    RGFW_window_mouseHold(window, {SCR_WIDTH, SCR_HEIGHT});

    u8 cursor[1] = {0};
    RGFW_window_setMouse(window, cursor, {1, 1}, 3);
    // RGFW_window_setMouseStandard(window, 0);

    if (RGL_loadGL3((RGLloadfunc)RGFW_getProcAddress)) {
        assert("Failed to initialize GLAD\n");
    }

	// wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// activer par defaut, mode remplie
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	std::cout << glGetString(GL_VERSION) << std::endl;

	// min 16 vec4
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nb of vertex attributes supported: " << nrAttributes << std::endl;

	glEnable(GL_DEPTH_TEST);

	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

	yaw = -90.0f;
	pitch = 0.0f;

    beginTime = RGFW_getTimeNS();

	return window;
}

void processEvent(RGFW_window *window) {
    while (RGFW_window_checkEvent(window)) {
        if (window->event.type == RGFW_quit || RGFW_isPressed(window, RGFW_Escape)) {
            RGFW_window_setShouldClose(window);
        }
        if (window->event.type == RGFW_windowResized) {
            SCR_WIDTH = window->r.w;
            SCR_HEIGHT = window->r.h;
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        }
        // qwerty key ;(

        float cameraSpeed = deltaTime * 4.0f;
        if (RGFW_isPressed(window, RGFW_ControlL))
            cameraSpeed *= 2.0f;

        if (RGFW_isPressed(window, RGFW_ShiftL))
            cameraPos.y -= cameraSpeed;
        if (RGFW_isPressed(window, RGFW_Space))
            cameraPos.y += cameraSpeed;

        if (RGFW_isPressed(window, RGFW_w))
            cameraPos += cameraSpeed * cameraFront;
        if (RGFW_isPressed(window, RGFW_s))
            cameraPos -= cameraSpeed * cameraFront;
        if (RGFW_isPressed(window, RGFW_a))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (RGFW_isPressed(window, RGFW_d))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (RGFW_isPressed(window, RGFW_p))
            cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);

        if (RGFW_isPressed(window, RGFW_z)){
            FOV = 5.0f;
            sensitivity = 0.05f;
        }else{
            FOV = 45.0f;
            sensitivity = 0.1f;
        }

        if (window->event.type == RGFW_mousePosChanged) {
            mouse_callback(window, window->event.point.x, window->event.point.y);
        }
    }
}

void framebuffer_size_callback(RGFW_window* window, int width, int height){
	std::cerr << window << std::endl;
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void mouse_callback(RGFW_window* window, double xpos, double ypos){
    (void) window;
	float xoffset = xpos;
	float yoffset = -ypos;
	// lastX = xpos;
	// lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw   += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f) pitch =  89.0f;
	if(pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void clearErr(){
	std::cout << "CLEAR ERROR" << std::endl;
	unsigned int err = glGetError();
	do{
		std::cout << err << std::endl;
		err = glGetError();
	}while (err);
	std::cout << "DONE CLEARING ERROR" << std::endl;
}

void getErr(){
	std::cout << glGetError() << std::endl;
}

void tearDown(){
	// glfwTerminate();
}

void frameTime(){
	timeLastFrame = currentTime;
	currentTime = (float)(RGFW_getTimeNS() - beginTime) / 1000 / 1000 / 1000;
	deltaTime = currentTime - timeLastFrame;
}

void renderLoop(RGFW_window* window){

	Shader shader1("shader/1.vs", "shader/1.fs");


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);  

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f), 
		glm::vec3( 2.0f,  5.0f, -15.0f), 
		glm::vec3(-1.5f, -2.2f, -2.5f),  
		glm::vec3(-3.8f, -2.0f, -12.3f),  
		glm::vec3( 2.4f, -0.4f, -3.5f),  
		glm::vec3(-1.7f,  3.0f, -7.5f),  
		glm::vec3( 1.3f, -2.0f, -2.5f),  
		glm::vec3( 1.5f,  2.0f, -2.5f), 
		glm::vec3( 1.5f,  0.2f, -1.5f), 
		glm::vec3(-1.3f,  1.0f, -1.5f)  
	};

	stbi_set_flip_vertically_on_load(1);

	int width, height, nrChannels;
	unsigned char *data = stbi_load("data/elephant.png", &width, &height, &nrChannels, 0);

	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		std::cout << "Failled to load texture" << std::endl;
	}
	stbi_image_free(data);

	std::cout << std::endl << std::endl << std::endl;
	for (int i=0; !RGFW_window_shouldClose(window); i++){
		frameTime();
		processEvent(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);

		glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++){
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i + std::sin(currentTime) * 100;
			if (i%2) angle *= -1;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader1.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		RGFW_window_swapBuffers(window);
		// glfwPollEvents();

		if (!(i%5)){
			puts("\033[4F");
			std::cout << "fps : " << (int)(1 / deltaTime) << "           " << std::endl;
			std::cout << "ms : " << deltaTime * 1000 << "           " << std::endl;
			std::cout << "lt : " << (RGFW_getTimeNS() - beginTime) / (1000*1000*1000) << "           " << std::endl;
		}

        // printf("test: %f\n", (float)(RGFW_getTimeNS() - beginTime) / 1000 / 1000);
        // printf("delt: %f\n", deltaTime);
	}

	clearErr();
	getErr();
}
