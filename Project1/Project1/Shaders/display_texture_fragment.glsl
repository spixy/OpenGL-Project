#version 430 core

layout (location = 0) out vec4 final_color;

in VertexData
{
	vec2 tex_coord;
} inData;

layout (binding = 0) uniform sampler2D tex;

void main()
{
	final_color = vec4(texture(tex, inData.tex_coord).rgb, 1.0);
}