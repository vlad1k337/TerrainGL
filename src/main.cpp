#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "terrain.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void proccesInput(GLFWwindow* window);

glm::vec3 cameraPos   = glm::vec3(0.0f, 3.0f,  0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

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
    
    glEnable(GL_DEPTH_TEST);

    unsigned int vertex     = compileShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int tesControl = compileShader("shaders/tcs.glsl", GL_TESS_CONTROL_SHADER);
    unsigned int tesEval    = compileShader("shaders/tes.glsl", GL_TESS_EVALUATION_SHADER);
    unsigned int fragment   = compileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
   

    unsigned int program = linkShader(4, vertex, fragment, tesControl, tesEval);

    Terrain terrain("assets/heightmap.png", program);
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
     
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while(!glfwWindowShouldClose(window))
    {
      glfwPollEvents();
      
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      ImGui::ShowDemoWindow();

      proccesInput(window);

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glUseProgram(program);
      
      model = glm::mat4(1.0f);
      view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
      projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 1000.0f);

      setUniformMatrix(program, "model", model);
      setUniformMatrix(program, "view", view);
      setUniformMatrix(program, "projection", projection);

      glBindVertexArray(VAO);
      terrain.drawTerrain();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      
      glfwSwapBuffers(window);
      
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate(); 
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void proccesInput(GLFWwindow *window)
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
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	cameraPos += cameraUp * speed;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	cameraPos -= cameraUp * speed;


}