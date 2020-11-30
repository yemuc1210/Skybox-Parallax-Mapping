#version 330 core
out vec4 color;				//输出一个4阶矩阵，颜色：某个片段的颜色
uniform vec3 objectColor;//uniform 结构体 它在编译过程中编译器会保留它的名字，然后程序员就可通过名字获取其在编译程序中的地址
void main()
{
	color =vec4(objectColor, 0.5f);  //RGB,最后alpha
}//fragment shader 片元着色器