#version 150

in float i_Speed;
in vec2 i_Polars;
in vec2 i_Pos;
in vec2 i_UV;

out float o_Speed;
out vec2 o_Polars;
out vec2 o_Pos;
out vec2 o_UV;

void main()
{
	o_Polars = i_Polars;
	o_Polars.x-= i_Speed;

	o_Pos.x = cos(o_Polars.x)*o_Polars.y;
	o_Pos.y = sin(o_Polars.x)*o_Polars.y;

	o_Speed = i_Speed;
	o_UV = i_UV;
}