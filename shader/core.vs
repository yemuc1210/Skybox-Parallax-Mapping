#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;     //传入计算得到的切线
layout (location = 4) in vec3 aBitangent;	//副切线

				
out  vec3 FragPos;
out  vec2 TexCoords;
out  vec3 TangentLightPos;
out  vec3 TangentViewPos;
out  vec3 TangentFragPos;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));   
    TexCoords = aTexCoords;   
    
    vec3 T = normalize(mat3(model) * aTangent);	//位移贴图在fs实现（每个frag表面位移都不同），
    vec3 B = normalize(mat3(model) * aBitangent);	//需计算观察者到方向向量
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));			//得到TBN矩阵,切线空间

    TangentLightPos = TBN * lightPos;		//计算这些位置方向
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}