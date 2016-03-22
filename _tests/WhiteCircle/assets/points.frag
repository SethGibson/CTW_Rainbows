#version 150
uniform vec4 u_Color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1,1,1,u_Color.a);
}