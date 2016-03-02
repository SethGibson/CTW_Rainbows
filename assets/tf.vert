#version 150
uniform float u_Elapsed;
uniform float u_SinMod;
uniform float u_CosMod;
uniform float u_SpeedMod;

in float i_Speed;
in vec2 i_Polars;
in vec2 i_UV;

out float o_Speed;
out vec2 o_Polars;
out vec2 o_Pos;
out vec2 o_UV;

void main()
{
	o_Polars = i_Polars;
	o_Polars.x-= i_Speed;

	float speedX = i_Speed*u_SpeedMod;
	if(i_Speed<0)
		o_Polars.y+=sin(u_Elapsed/u_SinMod+speedX);
	else
		o_Polars.y+=cos(u_Elapsed/u_CosMod+speedX);

	o_Pos.x = cos(o_Polars.x)*o_Polars.y;
	o_Pos.y = sin(o_Polars.x)*o_Polars.y;

	o_Speed = i_Speed;
	o_UV = i_UV;
}