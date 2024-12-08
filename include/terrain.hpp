#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders.hpp"

class Terrain
{
	public:
		float color[4]     = {1.0f, 1.0f, 1.0f, 1.0f};	
		float brightness = 0.0f;						// affects color whiteness
		float shininess = 32.0f;						// affects Blinn-Phong specular lighting
		
		float heightScale  = 64.0f;
		float heightOffset = 16.0f;

		glm::vec2 uTexelSize;		
		
		Terrain(const char* path, unsigned int shaderProgram)
		{
			this->shaderProgram = shaderProgram;
			loadHeightMap(path);
			constructVBO();
			constructVAO();

		}

		~Terrain()
		{

		}
		
		void renderTerrain()
		{
		    setUniformFloat(shaderProgram, "redComponent", color[0]);
			setUniformFloat(shaderProgram, "greenComponent", color[1]);
			setUniformFloat(shaderProgram, "blueComponent", color[2]);
			setUniformFloat(shaderProgram, "brightness", brightness);
			setUniformFloat(shaderProgram, "shininess", shininess);
			setUniformFloat(shaderProgram, "heightScale", heightScale);
			setUniformFloat(shaderProgram, "heightOffset", heightOffset);
			setUniformVec2(shaderProgram, "uTexelSize", uTexelSize);
			
			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, heightMap);
			glPatchParameteri(GL_PATCH_VERTICES, 4);	
			glDrawArrays(GL_PATCHES, 0, 4*res*res);
			glBindVertexArray(0);
		}

	private:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int shaderProgram;
		unsigned int heightMap;	

		const int unsigned res = 20;
	    int width, height, nrChanels;	
		unsigned char* heightMapData;

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
			uTexelSize = glm::vec2(1.0f/width, 1.0f/height);
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
