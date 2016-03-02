#version 150
uniform sampler2D u_SamplerRGB;

in vec2 UV;
out vec4 FragColor;

void main()
{
	FragColor = texture(u_SamplerRGB,UV);
	FragColor.a = 0.08f;
}