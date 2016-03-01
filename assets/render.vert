#version 150
#define M_PI 3.1415926535897932384626433832795

uniform mat4 ciModelViewProjection;

in vec2 o_Pos;
in vec2 o_UV;

out vec2 UV;
void main()
{
	gl_Position = ciModelViewProjection*vec4(o_Pos,0,1);
	gl_PointSize = 2.0f;
	UV = o_UV;
}