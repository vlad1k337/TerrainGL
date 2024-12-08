#ifndef POSTPROC_H
#define POSTPROC_H

#include <glad/glad.h>
#include "shaders.hpp"

static float quadVertices[24] =
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

static float kernelSharp[9] =
{
	  0, -1,  0,
	 -1,  5, -1,
	  0, -1,  0
};

static float kernelRidge[9] =
{
	 -1, -1, -1,
	 -1,  8, -1,
	 -1, -1, -1
};

static float kernelBlur[9] =
{
	 1, 2, 1,
	 2, 4, 2,
	 1, 2, 1
};

static float kernelIdentity[9] =
{
	 0, 0, 0,
	 0, 1, 0,
	 0, 0, 0
};

class PostProcess 
{
	public:
		PostProcess(const char* vertex, const char* fragment)
		{
			initTexturedQuad();
			initFrameBuffer();
			initProcessingProgram(vertex, fragment);
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
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		    glClearColor(1.0, 1.0, 1.0, 1.0);
		    glClear(GL_COLOR_BUFFER_BIT);

		    glUseProgram(processProgram);
		    glBindVertexArray(quadVAO);
		    glDisable(GL_DEPTH_TEST);
		    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
		    glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		void setKernel(ProcessingKernel kernel)
		{
			glUseProgram(processProgram);

			if(kernel == sharp)
				setUniformKernel(processProgram, "kernel", kernelSharp);
			if(kernel == ridge)
				setUniformKernel(processProgram, "kernel", kernelRidge);
			if(kernel == blur)
				setUniformKernel(processProgram, "kernel", kernelBlur);
			if(kernel == identity)
				setUniformKernel(processProgram, "kernel", kernelIdentity);

		}


		private:
		unsigned int quadVAO;
		unsigned int quadVBO;

		unsigned int Framebuffer;
		unsigned int FramebufferTexture;
		unsigned int Renderbuffer;

		unsigned int processProgram;

		void initTexturedQuad()
		{
			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);
			
			glGenBuffers(1, &quadVBO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

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
			glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FramebufferTexture, 0);

			glGenRenderbuffers(1, &Renderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffer);  
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Renderbuffer);
		}

		void initProcessingProgram(const char* vertex, const char* fragment)
		{
			unsigned int vertexShader   = compileShader(vertex, GL_VERTEX_SHADER);
			unsigned int fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);
		    processProgram = linkShader(2, vertexShader, fragmentShader);
		}

	
		

};




#endif
