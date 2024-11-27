#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders.h"

class Terrain
{
	public:
		float color[4]     = {1.0f, 1.0f, 1.0f, 1.0f};	
		float brightness = 0.0f;
		
		float heightScale  = 64.0f;
		float heightOffset = 16.0f;

		unsigned int VBO;
		unsigned int heightMap;

		glm::vec2 uTexelSize;		
		
		Terrain(const char* path, unsigned int shaderProgram)
		{
			loadHeightMap(path, shaderProgram);
			constructVBO();
		}

		~Terrain()
		{

		}
		
		void drawTerrain()
		{
			glPatchParameteri(GL_PATCH_VERTICES, 4);	
			glDrawArrays(GL_PATCHES, 0, 4*res*res);
		}

	private:
		const int unsigned res = 20;
		int width;
		int height;
		int nChanels;	
		unsigned char* heightMapData;

		void loadHeightMap(const char* path, unsigned int shaderProgram)
		{
			glGenTextures(1, &heightMap);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, heightMap);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			heightMapData = stbi_load(path, &width, &height, &nChanels, 0);
			uTexelSize = glm::vec2(1.0f/width, 1.0f/height);
			if(heightMapData)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, heightMapData);
				glGenerateMipmap(GL_TEXTURE_2D);

				setUniformInt(shaderProgram, "heightMap", 0);
				std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
			}
			else {
				std::cout << "Could not download the height map.";
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

};

#endif
