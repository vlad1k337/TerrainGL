#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders.hpp"


class Terrain
{
	public:
		float color[4]   = {1.0f, 1.0f, 1.0f, 1.0f};	
		float brightness = 0.0f;						// affects color whiteness
		float shininess  = 32.0f;						// affects Blinn-Phong specular lighting
		
		Terrain(unsigned int shaderProgram, int width = 1024, int height = 1024)
		{
			this->shaderProgram = shaderProgram;
			this->width = width;
			this->height = height;
			constructVBO();
			constructVAO();

		}

		~Terrain()
		{

		}
		
		void renderTerrain()
		{
			setUniformFloat(shaderProgram, "frequency", frequency);
			setUniformFloat(shaderProgram, "amplitude", amplitude);
			setUniformInt(shaderProgram, "octaves", octaves);

		    setUniformFloat(shaderProgram, "redComponent", color[0]);
			setUniformFloat(shaderProgram, "greenComponent", color[1]);
			setUniformFloat(shaderProgram, "blueComponent", color[2]);

			setUniformFloat(shaderProgram, "brightness", brightness);
			setUniformFloat(shaderProgram, "metallic", metallic);
			setUniformFloat(shaderProgram, "roughness", roughness);
			setUniformFloat(shaderProgram, "ambientOcclusion", ambientOcclusion);

			setUniformBool(shaderProgram, "gamma", gammaCorrection);
			setUniformBool(shaderProgram, "gooch", gooch);
			
			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, heightMap);
			glPatchParameteri(GL_PATCH_VERTICES, 4);	
			glDrawArrays(GL_PATCHES, 0, 4*res*res);
			glBindVertexArray(0);
		}

		void addWidgets()
		{
		    ImGui::ColorEdit4("Albedo", color); 
			ImGui::NewLine();	

			ImGui::Text("Noise Function Adjustments");
			ImGui::SliderFloat("Frequency", &frequency, 1.0f, 16.0f, "%1.0f");
			ImGui::SliderFloat("Amplitude", &amplitude, 1.0f, 48.0f, "%1.0f");
			ImGui::SliderInt("Octaves", &octaves, 1, 16, "%d");
			ImGui::NewLine();	
			
			ImGui::Text("Lighting Adjustments");
		    ImGui::SliderFloat("Brightness", &brightness, -1.0f, 1.0f, "%.1f");
		    ImGui::SliderFloat("Ambient", &ambientOcclusion, -1.0f, 1.0f, "%.1f");
		    ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.1f");

			static int e = 0;
			if(ImGui::RadioButton("PBR", &e, 0))
			{
				gooch = false;
			}
			ImGui::SameLine();	
			if(ImGui::RadioButton("Gooch", &e, 1))
			{
				gooch = true;
			}
			ImGui::Checkbox("Gamma Correction", &gammaCorrection);	
			ImGui::NewLine();	
		}

	private:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int shaderProgram;
		unsigned int heightMap;	

		const int unsigned res = 20;
	    int width, height, nrChanels;	
		unsigned char* heightMapData;

		float frequency = 1.0;
		float amplitude = 1.0;
		int octaves = 6;

		float metallic = 0.0;
		float roughness = 0.2;
		float ambientOcclusion = 0.1;

		bool gammaCorrection = true;
		bool gooch = false;

		void loadHeightMap(const char* path)
		{	
			glGenTextures(1, &heightMap);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, heightMap);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			heightMapData = stbi_load(path, &width, &height, &nrChanels, STBI_rgb_alpha);
			if(!heightMapData)
			{
				
				std::cout << "Could not download the height map.\n" << stbi_failure_reason();
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, heightMapData);
				glGenerateMipmap(GL_TEXTURE_2D);

				setUniformInt(shaderProgram, "heightMap", 0);
				std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
			}
			stbi_image_free(heightMapData);
		}

		void constructVBO()
		{
			std::vector<float> vertices;
			for(int i = 0; i <= res - 1; i++)
			{
				for(int j = 0; j <= res - 1; j++)
				{
					vertices.push_back(-width/2.0f + width*i/(float)res);
					vertices.push_back(0.0f);	
					vertices.push_back(-height/2.0f + height*j/(float)res);
					vertices.push_back(i/(float)res);
					vertices.push_back(j/(float)res);


					vertices.push_back(-width/2.0f + width*(i+1)/(float)res);
					vertices.push_back(0.0f);	
					vertices.push_back(-height/2.0f + height*j/(float)res);
					vertices.push_back((i+1)/(float)res);
					vertices.push_back(j/(float)res);


					vertices.push_back(-width/2.0f + width*i/(float)res);
					vertices.push_back(0.0f);	
					vertices.push_back(-height/2.0f + height*(j+1)/(float)res);
					vertices.push_back(i/(float)res);
					vertices.push_back((j+1)/(float)res);


					vertices.push_back(-width/2.0f + width*(i+1)/(float)res);
					vertices.push_back(0.0f);	
					vertices.push_back(-height/2.0f + height*(j+1)/(float)res);
					vertices.push_back((i+1)/(float)res);
					vertices.push_back((j+1)/(float)res);

				}
			}
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		}

		void constructVAO()
		{
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			 
		}

};

#endif
