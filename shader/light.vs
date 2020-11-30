#version 330 core
layout(location = 0) in vec3 position;   //location  in ���룬Ҫ����һ����������������position
uniform mat4 model;						//mat  matrix����
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
}										//vertex shader  ����������Ϣ