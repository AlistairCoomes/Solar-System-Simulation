#version 330 core

out vec4 FragColor;
uniform vec3 BodyColor;

void main()
{
   FragColor = vec4(BodyColor,1.0f);
}