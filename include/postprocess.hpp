#ifndef POSTPROC_H
#define POSTPROC_H

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shaders.hpp"

static float QuadVertices[24] =
{               
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};	

enum ProcessingKernel
{
	sharp, 
	ridge,
	blur,
	identity
};

const float KernelSharp[9] =
{
	  0, -1,  0,
	 -1,  5, -1,
	  0, -1,  0
};

const float KernelRidge[9] =
{
	 -1, -1, -1,
	 -1,  8, -1,
	 -1, -1, -1
};

const float KernelBlur[9] =
{
	 0.0625, 0.125, 0.0625,
	 0.125, 0.025, 0.125,
	 0.0625, 0.125, 0.0625
};

const float KernelIdentity[9] =
{
	 0, 0, 0,
	 0, 1, 0,
	 0, 0, 0
};

class PostProcess 
{
	public:
		PostProcess()
		{

		}

		PostProcess(const char* vertex, const char* fragment)
		{
			WindowWidth  = 800;
			WindowHeight = 600;
			initTexturedQuad();
			initFrameBuffer();
			initProcessingProgram(vertex, fragment);
			glUseProgram(ProcessProgram);
			setUniformInt(ProcessProgram, "screenTexture", 0);
			setUniformKernel(ProcessProgram, "kernel", KernelIdentity);
		}

		~PostProcess()
		{
			
		}
		
		static void checkFramebufferCompleteness()
		{
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer is not complete!" << std::endl;
			else
				std::cout << "Framebuffer is ready to framebuff!" << std::endl;
		}

		void prepareProcessed()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		    glEnable(GL_DEPTH_TEST);
		}

		void renderProcessed()
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, IntermediateFramebuffer);
			glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT);
		    glDisable(GL_DEPTH_TEST);

		    glUseProgram(ProcessProgram);
			glBindVertexArray(QuadVAO);
		    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
		    glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		void addWidgets()
		{
			ImGui::Text("Post Proccesing effects");
			static int e = 3;

			if(ImGui::RadioButton("Sharp", &e, 0))
			{
				setUniformKernel(ProcessProgram, "kernel", KernelSharp);
				std::cout << "Set sharp" << std::endl;
			}
			ImGui::SameLine();
			if(ImGui::RadioButton("Ridge", &e, 1))
			{
				setUniformKernel(ProcessProgram, "kernel", KernelRidge);
				std::cout << "Set sharp" << std::endl;
			}
			ImGui::SameLine();

			if(ImGui::RadioButton("Blur", &e, 2))
			{
				setUniformKernel(ProcessProgram, "kernel", KernelBlur);
				std::cout << "Set blur" << std::endl;
			}
			ImGui::SameLine();

			if(ImGui::RadioButton("Identity", &e, 3))
			{
				setUniformKernel(ProcessProgram, "kernel", KernelIdentity);
				std::cout << "Set identity" << std::endl;
			}

		}

		inline void framebuffersResize(int width, int height)
		{
			glDeleteFramebuffers(1, &Framebuffer);
			glDeleteFramebuffers(1, &IntermediateFramebuffer);
			glDeleteRenderbuffers(1, &Renderbuffer);
			glDeleteTextures(1, &FramebufferTexture);
			glDeleteTextures(1, &IntermediateFramebufferTexture);

			WindowWidth = width;
			WindowHeight = height;

			initFrameBuffer();	
		}


	private:
		int WindowWidth;
		int WindowHeight;

		unsigned int QuadVAO;
		unsigned int QuadVBO;

		unsigned int Framebuffer;
		unsigned int FramebufferTexture;
		unsigned int Renderbuffer;

		unsigned int IntermediateFramebuffer;
		unsigned int IntermediateFramebufferTexture;

		unsigned int ProcessProgram;

		void initTexturedQuad()
		{
			glGenVertexArrays(1, &QuadVAO);
			glBindVertexArray(QuadVAO);
			
			glGenBuffers(1, &QuadVBO);
			glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), &QuadVertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}

		void initFrameBuffer()
		{
			glGenFramebuffers(1, &Framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
			
			glGenTextures(1, &FramebufferTexture);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, FramebufferTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, WindowWidth, WindowHeight, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, FramebufferTexture, 0);

			glGenRenderbuffers(1, &Renderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffer);  
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Renderbuffer);
			
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			checkFramebufferCompleteness();


			glGenFramebuffers(1, &IntermediateFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, IntermediateFramebuffer);

			glGenTextures(1, &IntermediateFramebufferTexture);
			glBindTexture(GL_TEXTURE_2D, IntermediateFramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, IntermediateFramebufferTexture, 0);

			checkFramebufferCompleteness();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

		void initProcessingProgram(const char* vertex, const char* fragment)
		{
			unsigned int vertexShader   = compileShader(vertex, GL_VERTEX_SHADER);
			unsigned int fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);
		    ProcessProgram = linkShader(2, vertexShader, fragmentShader);
		}

	
		

};




#endif
