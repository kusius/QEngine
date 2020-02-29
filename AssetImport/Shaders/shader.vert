#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;



void main()
{
	
	FragPos = vec3(model * vec4(aPos, 1.0));//get the vertex position in world coordinates
	//transform the normal vectors to world coords
	//must apply the normal matrix transform (the inverse and transpose of the model and NOT the model itself)
	//also cast to mat3 to ensure the translation properties are lost 
	//inverse is VERY COSTLY!!! Do it in the cpu and pass it as uniform
	//TODO: Multiply trick for transpose inverse
	Normal = mat3(transpose(inverse(model))) * aNormal ;

	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}