#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "terrain.h"
#include "gui.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xpos, double ypos);

void proccesInput(GLFWwindow* window);

glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

float lastOffsetX = 400, lastOffsetY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

static bool cursorBlocked = true;

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, " ", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create a window\n";
		glfwTerminate();
		return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
		return -1;
    }

	initGui(window);

    glEnable(GL_DEPTH_TEST);

    unsigned int vertex     = compileShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int tesControl = compileShader("shaders/tcs.glsl", GL_TESS_CONTROL_SHADER);
    unsigned int tesEval    = compileShader("shaders/tes.glsl", GL_TESS_EVALUATION_SHADER);
    unsigned int fragment   = compileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    unsigned int program = linkShader(4, vertex, fragment, tesControl, tesEval);

	Terrain* terrain;

	if(argc > 1)
		terrain = new Terrain(argv[1], program);
	else {
		terrain = new Terrain("assets/iceland_heightmap.png", program);
		std::cout << "Using default height-map: assets/iceland_heightmap.png\n";
	}
	

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
     
	while(!glfwWindowShouldClose(window))
	{
	  glfwPollEvents();

	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(program);
	  addWidgetsGui(terrain);
    
	  model = glm::mat4(1.0f);
      view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
      projection = glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 1000.0f);

      setUniformMatrix(program, "model", model);
      setUniformMatrix(program, "view", view);
      setUniformMatrix(program, "projection", projection);
	
	  setUniformFloat(program, "redComponent", terrain->color[0]);
	  setUniformFloat(program, "greenComponent", terrain->color[1]);
	  setUniformFloat(program, "blueComponent", terrain->color[2]);
	  setUniformFloat(program, "brightness", terrain->brightness);
	  setUniformFloat(program, "shininess", terrain->shininess);

	  setUniformFloat(program, "heightScale", terrain->heightScale);
	  setUniformFloat(program, "heightOffset", terrain->heightOffset);
	  setUniformVec2(program, "uTexelSize", terrain->uTexelSize);

	  setUniformVec3(program, "cameraPos", cameraPos);

	  glBindVertexArray(VAO);
      terrain->drawTerrain();
	  renderGui();
		
	  proccesInput(window);
      glfwSwapBuffers(window);
      
    }

	delete terrain;

	cleanGui();
    glfwTerminate(); 
    return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	cameraFront = glm::vec3(0.0, 0.0, -1.0);
	lastOffsetX = width/2;
	lastOffsetY = height/2;
	glfwSetCursorPos(window, width/2, height/2);	
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float currentOffsetX = xpos - lastOffsetX;
	float currentOffsetY  = -(ypos - lastOffsetY);

	if(cursorBlocked == false)
	{
		lastOffsetX = xpos;
		lastOffsetY = ypos;
		return;
	}

	currentOffsetX *= 0.1f;
	currentOffsetY *= 0.1f;

	lastOffsetX = xpos;
	lastOffsetY = ypos;

	yaw += currentOffsetX;
	pitch += currentOffsetY;

	if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}

void proccesInput(GLFWwindow* window)
{
  const float speed = 0.5f;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraFront * speed;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraFront * speed;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	cameraPos += cameraUp * speed;

  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
  {
	cursorBlocked = false;
  	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
  }

  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
  {
    cursorBlocked = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

}


