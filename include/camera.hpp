#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Direction 
{
	forward,
	backward,
	up,
	left,
	right
};

class Camera
{
	public:
		Camera() {}

		Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) : cameraPos(pos), cameraFront(front), cameraUp(up) {}
		
		~Camera() {}
		
		glm::mat4 getLookAt()
		{
			return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}

		glm::mat4 getProjection()
		{
			return glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 1000.0f);
		}

		glm::vec3 getCameraPos()
		{
			return cameraPos;
		}		

		void setCameraFront(glm::vec3 front)
		{
			cameraFront = front;
		}

		void processMovement(Direction direction, float deltaTime)
		{
			float cameraSpeed = speed * deltaTime;

			if(direction == forward)
				cameraPos += cameraFront * cameraSpeed;
			if(direction == backward)
				cameraPos -= cameraFront * cameraSpeed;
			if(direction == up)
				cameraPos += cameraUp * cameraSpeed;
		    if(direction == left)
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			if(direction == right)
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		void processMouse(float xoffset, float yoffset)
		{
			xoffset *= 0.1;
			yoffset *= 0.1;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)  pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(front);
		}

		void processScroll(float xoffset, float yoffset)
		{
			fov -= (float)yoffset;
			if (fov < 1.0f)
				fov = 1.0f;
			if (fov > 90.0f)
				fov = 90.0f; 
		}

	private:
		glm::vec3 cameraPos   = glm::vec3(0.0f, 50.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

		float yaw = -90.0f;
		float pitch = 0.0f;
		float fov = 45.0f;

		const float speed = 35.0f;

};

#endif
