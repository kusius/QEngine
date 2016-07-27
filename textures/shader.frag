#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform float mixRatio;

void main()
{
color = mix(texture(ourTexture, TexCoord), texture(ourTexture1, vec2(TexCoord.x, 1.0f - TexCoord.y)), mixRatio);
}
