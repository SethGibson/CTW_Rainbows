#version 150
uniform mat4 ciModelViewProjection;
uniform float u_PointSize;

in vec2 o_Pos;
in vec2 o_UV;

out vec2 UV;
void main()
{
	gl_Position = ciModelViewProjection*vec4(o_Pos,0,1);
	gl_PointSize = u_PointSize;
	UV = o_UV;
}