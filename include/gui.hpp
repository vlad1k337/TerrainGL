#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

float plotFPSGraph[60] = {0.0};
int plotFPSPos = 0;

void initGui(GLFWwindow* window);
void setGuiStyle();
void renderGui();
void cleanGui();

void initGui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    ImGui_ImplGlfw_InitForOpenGL(window, true);         
    ImGui_ImplOpenGL3_Init();
	setGuiStyle();
}

void setGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();	

	style.TabRounding = 8.f;
	style.FrameRounding = 8.f;
	style.GrabRounding = 8.f;
	style.WindowRounding = 8.f;
	style.PopupRounding = 8.f;	
}

void startGuiRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();	
	ImGui::Begin("Configuration");
}

void plotFPS()
{
	  ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

	  if(plotFPSPos > 59) 
		  plotFPSPos = 0;

	  plotFPSGraph[plotFPSPos++] = ImGui::GetIO().Framerate;
	  ImGui::PlotLines("##", plotFPSGraph, 60, 0, NULL, 0.0f, 60.0f, ImVec2(0.0f, 30.0f));
}

void renderGui()
{
	  ImGui::End();
	  ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

#endif
