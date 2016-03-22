#version 150
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;

void main()
{
	gl_PointSize =3.0f;
	gl_Position = ciModelViewProjection * ciPosition;
}