#version 330 core
out vec4 FragColor;

in  vec3 FragPos;
in  vec2 TexCoords;
in  vec3 TangentLightPos;
in  vec3 TangentViewPos;
in  vec3 TangentFragPos;

uniform sampler2D diffuseMap;  
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

//返回经位移的纹理坐标
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	/*****
	float height =  texture(depthMap, texCoords).r;    //texCoords从高度贴图中来采样出当前fragment高度H(A)
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);//viewDir向量除以它的z元素，用fragment的高度对它进行缩放
    return texCoords - p;					//height_scale进行额外的控制，因为视差效果如果没有一个缩放参数通常会过于强烈
	******/

    const float minLayers = 8;					//深度层次，陡峭视差贴图引进
    const float maxLayers = 32;
									//根据viewDir和正z方向的点乘，调整样本数量
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    
    float layerDepth = 1.0 / numLayers;				// 计算每一层的层大小，均分
    
    float currentLayerDepth = 0.0;					// 当前层深度
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale;		//沿着p方向向下移动
    vec2 deltaTexCoords = P / numLayers;
  
    vec2  currentTexCoords     = texCoords;				//初始化值
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;	//表面深度的纹理坐标

    while(currentLayerDepth < currentDepthMapValue)
    {
       
        currentTexCoords -= deltaTexCoords;			// 沿着p向量方向更新纹理坐标
      
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;    // 根据当前纹理坐标获取深度值
       
        currentLayerDepth += layerDepth;   // 得到下一层深度
    }
	//********视差遮蔽映射，线性插值
    // 获取交叉前的纹理坐标
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // 获取交叉前后的深度
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // 线性插值
    float weight = afterDepth / (afterDepth - beforeDepth);	//计算权重
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{           
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);//切线空间计算
    vec2 texCoords = TexCoords;
		 //****纹理坐标超出了0到1的范围进行采样，根据纹理的环绕方式导致了不真实的结果。
    texCoords = ParallaxMapping(TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;			//超过坐标范围的就丢弃掉

    vec3 normal = texture(normalMap, texCoords).rgb;	//从norm图获取法线分量
    normal = normalize(normal * 2.0 - 1.0);				//标准化
   
    
    vec3 color = texture(diffuseMap, texCoords).rgb;		// 获取diffuse color
    
    vec3 ambient = 0.1 * color;								// 环境
   
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);			//diffuse影响，取大于0的值
    vec3 diffuse = diff * color;							// diffuse分量
   
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);       //Blinn-Phong 半角 (light view)（viewPos 与 反射分量大于90问题解决）
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);  //影响
    vec3 specular = vec3(0.2) * spec;				// specular   分量

	vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}