#include <iostream>

//GLEW              
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
// SOIL2
#include "SOIL2/SOIL2.h"
#include "SOIL2/stb_image.h"
//glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader.h"
#include"Camera.h"
#include"Model.h"
#include"Mesh.h"

const GLint WIDTH = 800, HEIGHT = 720;
float heightScale = 0.1;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void DoMovement();

void renderQuad();
void renderQuad1();
void renderQuad_1();
Camera  camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool	firstMouse = true;

bool keys[1024];

float kR = 0.0f;				//*******R键触发的变量
float R = 1.0f, G = 1.0f, B = 1.0f;
glm::vec3 lightPos(-0.3f, 0.0f, 1.5f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//立方体坐标,使用索引绘图，共24个点，无需36个
GLfloat vertices[24][8] = {
	//前
{-1.0f,1.0f,2.0f, },
{-1.0f,-1.0f,2.0f,},
{1.0f,-1.0f,2.0f,},
{1.0f,1.0f,2.0f,},
//后
{-1.0f,1.0f,0.0f,	},
{-1.0f,-1.0f,0.0f,	},
{1.0f,-1.0f,0.0f,	 },
{1.0f,1.0f,0.0f,	},
//右
{1.0f,1.0f,2.0f,	},
{1.0f,-1.0f,2.0f,	},
{1.0f,-1.0f,0.0f,	},
{1.0f,1.0f,0.0f,	},
//左
{-1.0f,1.0f,0.0f,	},
{-1.0f,-1.0f,0.0f,	},
{-1.0f,-1.0f,2.0f,	},
{-1.0f,1.0f,2.0f,	},
//下
{-1.0f,-1.0f,2.0f,	},
{-1.0f,-1.0f,0.0f,	},
{1.0f,-1.0f,0.0f,	},
{1.0f,-1.0f,2.0f,	},
	//上
{-1.0f,1.0f,0.0f,	},
{-1.0f,1.0f,2.0f,	},
{1.0f,1.0f,2.0f,	},
{1.0f,1.0f,0.0f,	}
};
//法线信息
GLfloat normVertices[6][3] = {
	//前	
	{0.0f, 0.0f, 1.0f},
	//后
	{0.0f, 0.0f, -1.0f},
	//右
	{1.0f, 0.0f, 0.0f},
	//左
	{-1.0f, 0.0f, 0.0f},
	//下
	{0.0f, -1.0f, 0.0f},
	//上
	{0.0f, 1.0f, 0.0f}
};
GLfloat uvVertices[24][2] = {
	//前
{0.0f,1.0f},
{0.0f,0.0f},
{1.0f,0.0f},
{1.0f,1.0f},
//后
{0.0f, 1.0f},
{0.0f,0.0f},
{1.0f,0.0f},
{1.0f,1.0f},
//右
{1.0f,1.0f},
{0.0f,1.0f},
{0.0f,0.0f},
{1.0f,0.0f},
//左
{1.0f,1.0f},
{0.0f,1.3f},
{0.0f,2.0f},
{1.0f,2.0f},

//下
{0.0f,2.0f},
{0.0f,1.0f},
{1.0f,1.0f},
{1.0f,2.0f},
//上
{0.0f,0.0f},
{0.0f,1.0f},
{1.0f,1.0f},
{1.0f,0.0f }
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);//*********获取窗口信息

	glfwMakeContextCurrent(window);                         //*******相当于“聚焦”到当前窗口
	glfwSetKeyCallback(window, KeyCallback);				//******以下为注册回调函数
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, ScrollCallback);			//***********响应滚轮的回调函数，此处为注册回调函数

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);		//开启深度测试
	
	Shader skyShader("shader/skybox.vs", "shader/skybox.frag"); 
	Shader modelShader("shader/modelLoad.vs", "shader/modelLoad.frag");
	Shader shader("shader/core.vs", "shader/core.frag");	//*****视差贴图的着色器对象
	Shader lightShader("shader/light.vs", "shader/light.frag");
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	//******skybox VAO VBO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//******光源 VAO VBO
	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//********纹理加载
	vector<std::string> faces
	{
		"mp_tf/mp_tf/thefog_ft.tga",
		"mp_tf/mp_tf/thefog_bk.tga",
		"mp_tf/mp_tf/thefog_up.tga",
		"mp_tf/mp_tf/thefog_dn.tga",
		"mp_tf/mp_tf/thefog_rt.tga",
		"mp_tf/mp_tf/thefog_lf.tga",
	};
	//*******天空盒纹理加载部分
	unsigned int skyboxTexture;
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	int width, height, nrChannels;
	//一个for循环，实现为每个面都加载一个纹理
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGBA);
		if (data)
		{ //纹理目标-6个面 默认多级渐远纹理级别 纹理存储级别 
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}
	//设置纹理过滤方式和环绕方式
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//******以下为parallax_maxpping部分的纹理加载
	//************diffuseMap
	unsigned int diffuseMap;
	glGenTextures(1, &diffuseMap);
	int nrComponents;
	unsigned char *data = stbi_load("resources/bricks2.jpg", &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;		//纹理存储级别，视通道数量定
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);	//多级渐远纹理的级别

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "resources/bricks2.jpg" << std::endl;
		SOIL_free_image_data(data);
	}
	//************heightMap
	unsigned int heightMap;    //整个平面需要根据从高度图采样出来的高度值进行位移
	glGenTextures(1, &heightMap);
	data = stbi_load("resources/bricks2_disp.jpg", &width, &height, &nrComponents, 0);			//使用SOIL_load_image  加载失败，故换stbi_load
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, heightMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "resources/bricks2_disp.jpg" << std::endl;
		stbi_image_free(data);
	}
	//************normalMap
	unsigned int normalMap;
	glGenTextures(1, &normalMap);
	data = stbi_load("resources/bricks2_normal.jpg", &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, normalMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "resources/bricks2_normal.jpg" << std::endl;
		SOIL_free_image_data(data);
	}

	Model ourModel("nanosuit_reflection/nanosuit.obj");
	//*****视差贴图部分
	shader.Use();
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseMap"), 0);//0 1 2 是纹理号
	glUniform1i(glGetUniformLocation(shader.Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(shader.Program, "depthMap"), 2);
	//*****天空盒
	skyShader.Use();
	glUniform1i(glGetUniformLocation(skyShader.Program, "skybox"), 0);

	glm::mat4 view = glm::mat4(1.0f);//视角矩阵
	glm::mat4 projection;		//投影矩阵
	//渲染循环
	while (!glfwWindowShouldClose(window)) {

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//****清除深度缓冲，此时深度值为1.0f
		//响应
		DoMovement();
		//*****先画模型、物体，最后画天空盒，实现一定优化
		//****模型
		modelShader.Use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.5f, -1.55f, 0.0f));			// 适当下调位置
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));					// 适当缩小模型
		view = camera.GetViewMatrix();											//获取view矩阵
						//投影矩阵公式    要传入弧度制，传入角度值会引起上下颠倒问题
									//fov	aspect表示裁剪面的宽w高h比，这个影响到视野的截面有多大 近裁面 远裁面
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(modelShader.Program, "envText"), 3);	//设置的ambient纹理，用于模型的反射贴图
		shader.setVec3("cameraPos", camera.GetPosition());						//反射贴图需要反射方向
		ourModel.Draw(modelShader);												//调用函数绘制模型
		//***********渲染视差贴图的物体
		projection = glm::perspective(glm::radians(camera.GetZoom()), static_cast<GLfloat>(screenWidth) / static_cast<GLfloat>(screenHeight), 0.1f, 1000.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f)); // 适当下调位置
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * kR), glm::normalize(glm::vec3(0.0, 0.8, 1.0))); // rotate the quad to show parallax mapping from multiple directions
		model = glm::scale(model, glm::vec3(0.3f));
		shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(shader.Program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(glGetUniformLocation(shader.Program, "heightScale"), heightScale);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);
		renderQuad();
			//*******第二面
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);
		renderQuad1();
		//***********光源
		lightShader.Use();
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * kR), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
		model = glm::scale(model, glm::vec3(0.02f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(glGetUniformLocation(lightShader.Program, "objectColor"), R, G, B);
		glBindVertexArray(lightVAO);	
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//**********开始画skybox
		glDepthFunc(GL_LEQUAL);							//****改变深度测试的方式为  小于等于，因为skybox的深度为最大的1.0
		skyShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); //*****去除view矩阵的移动部分，只保留其上三角矩阵
		glUniformMatrix4fv(glGetUniformLocation(skyShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(skyShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(skyboxVAO);	//绘制skybox
		glActiveTexture(GL_TEXTURE3);		//更改纹理单元号为3号单元
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);							//****设置深度测试的方式为默认，用于下一次while循环的绘制物体
		
		glfwSwapBuffers(window);					//交换缓存
	}
	glDeleteVertexArrays(1, &lightVAO);				//解绑
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteBuffers(1, &skyboxVBO);

	glfwTerminate();		//释放glfw资源
	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		// 顶点位置
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// 纹理坐标，二维
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// 法向量，包含方向的
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		//***计算两个三角形的切线T  和副切线 B
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		//***第一个三角形计算
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		//****第二个三角形
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); 
}
unsigned int quadVAO1 = 0;
unsigned int quadVBO1;
void renderQuad1()
{
	int i = 0;
	if (quadVAO1 == 0)
	{
		std::cout << i++ << std::endl;
		// positions
		glm::vec3 pos1(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 2.0f);
		glm::vec3 pos3(1.0f, -1.0f, 2.0f);
		glm::vec3 pos4(1.0f, -1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 0.0f);
		glm::vec2 uv2(0.0f, 1.0f);
		glm::vec2 uv3(1.0f, 1.0f);
		glm::vec2 uv4(1.0f, 0.0f);
		// normal vector
		glm::vec3 nm(0.0f, 1.0f, 0.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		
		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO1);
		glGenBuffers(1, &quadVBO1);
		glBindVertexArray(quadVAO1);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
//处理键盘输入
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}
//响应鼠标移动
void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = yPos - lastY;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
void ScrollCallback(GLFWwindow *window, double xPos, double yPos)
{
	camera.ProcessMouseScroll(yPos);

}
//响应键盘输入
void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {    //
	//	lightPos2.x += 0.002f;
		camera.ProcessKeyboard(FORWARD, deltaTime);  //w无用

	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
		//	lightPos.x -= 0.001f;
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {  //往外移动
	//	lightPos.z += 0.001f;
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		//	lightPos.z -= 0.002f;
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_Z]) {
		lightPos.y += 0.001f;
	}
	if (keys[GLFW_KEY_X]) {
			lightPos.y -= 0.002f;
	}
	if (keys[GLFW_KEY_R]) {		//*********新增的键盘操作
		if (kR >= 0)			//按R键旋转，增加旋转速度
			kR += 0.001f;
		else if (kR < 0)
			kR = 0;
	}
	if (keys[GLFW_KEY_N]) {		//**********N键减小旋转速度
		if (kR >= 0)
			kR -= 0.001f;
		else if (kR < 0)		//**********最小只能是0
			kR = 0;
	}
	if (keys[GLFW_KEY_M]) {
		std::cout << "M" << std::endl;
		if (R + 0.02f <= 1 && G + 0.02f <= 1 && B + 0.02f <= 1) {
			R += 0.02f;
			G += 0.1f;
			B += 0.01f;
		}
	}
	if (keys[GLFW_KEY_Q])
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
	}
	if (keys[GLFW_KEY_E])
	{
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
	}
	if (keys[GLFW_KEY_U]) {		//**********处理光源移动
		lightPos.x += 0.002f;
	}
	if (keys[GLFW_KEY_I]) {
		lightPos.x -= 0.002f;
	}
	if (keys[GLFW_KEY_O]) {
		lightPos.z += 0.002f;
	}
	if (keys[GLFW_KEY_P]) {
		lightPos.z -= 0.002f;
	}

}

