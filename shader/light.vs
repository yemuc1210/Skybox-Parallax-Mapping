#version 330 core
layout(location = 0) in vec3 position;   //location  in 输入，要输入一个三阶向量给变量position
uniform mat4 model;						//mat  matrix矩阵
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
}										//vertex shader  处理顶点信息