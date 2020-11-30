#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 color;

uniform samplerCube envText;			// 环境纹理
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_reflection1;	// 反射map
uniform vec3 cameraPos;					//需要位置计算反射方向
/*
	通过加载模型中的reflection map，
	决定渲染的物体中哪部分需要做反射环境贴图以及环境贴图的强度系数，
	而不是像上面球体那样，整个执行反射环境贴图
*/
void main()
{
    // diffuse
    vec4 diffuse = texture(texture_diffuse1, TexCoords);   //漫反射分量
    
    // specular
   // vec4 specular = texture(texture_specular1, TexCoords);		//镜面反射分量
    
	//norm 
    vec4 normal = texture(texture_normal1, TexCoords);			//法向分量，想做法线贴图，未成功，此处未传值进来，可忽略
    
	//reflect
	vec3 viewDir = normalize(FragPos - cameraPos);       //反射
	vec3 reflectDir = reflect(viewDir,  normalize(Normal));
	float	relefctIntensity = texture(texture_reflection1, TexCoords).r; 
	vec4 reflect = texture(envText, reflectDir) * relefctIntensity;	// 使用反射向量采样环境纹理 使用强度系数控制
	
    color = diffuse  + reflect + normal;			//反射贴图输出
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
