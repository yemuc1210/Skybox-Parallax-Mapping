

#pragma once

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW = -90.0f; //偏航角
const GLfloat PITCH = 0.0f;//俯仰角
const GLfloat SPEED = 6.0f;//摄像机速度
const GLfloat SENSITIVITY = 0.25f;//初始灵敏度
const GLfloat ZOOM = 75.0f;//初始视野

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for OpenGL
class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) :front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY),zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw = YAW, GLfloat pitch = PITCH) :front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->movementSpeed * deltaTime;
		
		if (direction == FORWARD) {
			this->position += this->front * velocity;
		}

		if (direction == BACKWARD) {
			this->position -= this->front * velocity;
		}

		if (direction == LEFT) {
			this->position -= this->right * velocity;
		}

		if (direction == RIGHT) {
			this->position += this->right * velocity;
		}
	}

	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPith = true)
	{
		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;

		this->yaw += xOffset;
		this->pitch += yOffset;

		if (constrainPith) {
			if (this->pitch > 89.0f) {
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f) {
				this->pitch = -89.0f;
			}

		}
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(double yoffset)             //*************响应滚轮的函数，主要是改变zoom值
	{//实际上是通过改变视野field of view  fov，来改变物体大小，原理是近大远小，视野近，物体就显得大
		std::cout << this->zoom << std::endl;
		if (this->zoom >= 1.0f && this->zoom <= 75.0f)
			this->zoom -= yoffset;
		if (this->zoom <= 1.0f)
			this->zoom = 1.0f;
		if (this->zoom >= 75.0f)
			this->zoom = 75.0f;
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	GLfloat GetZoom()
	{
		return this->zoom;
	}

	glm::vec3 GetPosition()
	{
		return this->position;
	}

	void changePosition(glm::vec3 lightPos) {
		this->position = lightPos;
	}
private:
	glm::vec3 position;//摄像机位置
	glm::vec3 front;//摄像机方向向量
	glm::vec3 up;//摄像机上轴
	glm::vec3 right;//摄像机右轴
	glm::vec3 worldUp;//世界空间上轴

	GLfloat yaw;//偏航角
	GLfloat pitch;//俯仰角

	GLfloat movementSpeed;//移动速度
	GLfloat mouseSensitivity;//灵敏度
	GLfloat zoom;//视野
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
		front.y = sin(glm::radians(this->pitch));
		front.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
		this->front = glm::normalize(front);

		this->right = glm::normalize(glm::cross(this->front, this->worldUp));

		this->up = glm::normalize(glm::cross(this->right, this->front));

	}
};






