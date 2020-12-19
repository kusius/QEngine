#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in mat4 aIntanceModelMatrix; // 4,5,6,7

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * aInstanceModelMatrix * vec4(aPos, 1.0f);
  TexCoords = aTexCoords;
}