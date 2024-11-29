#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "terrain.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
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

int main()
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
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
		return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    ImGui_ImplGlfw_InitForOpenGL(window, true);         
    ImGui_ImplOpenGL3_Init();
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.TabRounding = 8.f;
	style.FrameRounding = 8.f;
	style.GrabRounding = 8.f;
	style.WindowRounding = 8.f;
	style.PopupRounding = 8.f;	
    
    glEnable(GL_DEPTH_TEST);

    unsigned int vertex     = compileShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int tesControl = compileShader("shaders/tcs.glsl", GL_TESS_CONTROL_SHADER);
    unsigned int tesEval    = compileShader("shaders/tes.glsl", GL_TESS_EVALUATION_SHADER);
    unsigned int fragment   = compileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    unsigned int program = linkShader(4, vertex, fragment, tesControl, tesEval);

    Terrain* terrain = new Terrain("assets/heightmap.png", program);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
     

	float FPSplot[60] = {0.0f};
	int plotPos = 0;


	while(!glfwWindowShouldClose(window))
	{
	  glfwPollEvents();
		  
	  ImGui_ImplOpenGL3_NewFrame();
	  ImGui_ImplGlfw_NewFrame();
	  ImGui::NewFrame();
		
	  ImGui::Begin("Configuration");
	  ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

	  if(plotPos > 59) plotPos = 0;
	  FPSplot[plotPos++] = ImGui::GetIO().Framerate;
	  ImGui::PlotLines("##", FPSplot, 60, 0, NULL, 0.0f, 60.0f, ImVec2(0.0f, 30.0f));

	  static int e = 0;
	  if(ImGui::RadioButton("Normal", &e, 0))
	  {
		  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  }
	  ImGui::SameLine();
	  if(ImGui::RadioButton("Wireframe", &e, 1))
	  {
		  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  }
	  ImGui::SameLine();
	  if(ImGui::RadioButton("Points", &e, 2))
	  {
		  glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	  }

	  ImGui::ColorEdit4("Color", terrain->color);  

	  ImGui::SliderFloat("Brightness", &terrain->brightness, -1.0f, 1.0f, "%.1f");
	  ImGui::SliderFloat("Height Scale", &terrain->heightScale, 16.0f, 128.0f, "%1.0f");
	  ImGui::SliderFloat("Height Offset", &terrain->heightOffset, 0.0f, 32.0f, "%1.0f");
	  ImGui::SliderFloat("FOV", &fov, 45.0f, 90.0f, "%1.0f");
		
	  ImGui::End();

	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(program);
    
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

	  setUniformFloat(program, "heightScale", terrain->heightScale);
	  setUniformFloat(program, "heightOffset", terrain->heightOffset);

	  glBindVertexArray(VAO);
      terrain->drawTerrain();


      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
	  proccesInput(window);
      glfwSwapBuffers(window);
      
    }

	delete terrain;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate(); 
    return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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


