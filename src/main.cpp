#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shaders.hpp"
#include "terrain.hpp"
#include "gui.hpp"
#include "postprocess.hpp"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xpos, double ypos);

void proccesInput(GLFWwindow* window);
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

PostProcess postProcessing;

Camera camera;
static bool cursorBlocked = true;

float lastOffsetX = 400.0f, lastOffsetY = 300.0f;

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
	glEnable(GL_DEPTH_TEST | GL_MULTISAMPLE);
	postProcessing = PostProcess("shaders/procvertex.glsl", "shaders/procfragment.glsl");
	initGui(window);

    unsigned int vertex     = compileShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int tesControl = compileShader("shaders/tcs.glsl", GL_TESS_CONTROL_SHADER);
    unsigned int tesEval    = compileShader("shaders/tes.glsl", GL_TESS_EVALUATION_SHADER);
    unsigned int fragment   = compileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    unsigned int program = linkShader(4, vertex, fragment, tesControl, tesEval);

	Terrain* terrain;
	if(argc > 2)
		terrain = new Terrain(program, std::atoi(argv[1]), std::atoi(argv[2]));
	else {
		terrain = new Terrain(program);
		std::cout << "Using default map of size 1024x1024"  << std::endl;
	}

 
	while(!glfwWindowShouldClose(window))
	{
	  proccesInput(window);

	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  postProcessing.prepareProcessed();

      glUseProgram(program);
	  model = glm::mat4(1.0f);
      view = camera.getLookAt();
	  projection = camera.getProjection();

      setUniformMatrix(program, "model", model);
      setUniformMatrix(program, "view", view);
      setUniformMatrix(program, "projection", projection);
	  setUniformVec3(program, "cameraPos", camera.getCameraPos());

      terrain->renderTerrain();
	  postProcessing.renderProcessed();
		
	  startGuiRender();
	  plotFPS();
	  terrain->addWidgets();
	  postProcessing.addWidgets();
	  renderGui();

      glfwSwapBuffers(window);
	  glfwPollEvents();
    }

	delete terrain;
	
	cleanGui();
	glfwTerminate();
    return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	postProcessing.framebuffersResize(width, height);
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

	lastOffsetX = xpos;
	lastOffsetY = ypos;

	camera.processMouse(currentOffsetX, currentOffsetY);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processScroll(xoffset, yoffset);
}

void proccesInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	camera.processMovement(forward); 

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	  camera.processMovement(backward);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	  camera.processMovement(up);

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	  camera.processMovement(left);

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	  camera.processMovement(right);

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


