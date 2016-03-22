#version 150
uniform mat4 ciModelViewProjection;

in vec4 ciColor;
in vec4 ciPosition;

out vec4 VertColor;

void main()
{
	VertColor = ciColor;
	gl_Position = ciModelViewProjection * ciPosition;
}