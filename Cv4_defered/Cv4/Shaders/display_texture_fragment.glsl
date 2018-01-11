#version 430 core

layout (location = 0) out vec4 final_color;

in VertexData
{
	vec2 tex_coord;
} inData;

layout (binding = 0) uniform sampler2D input_tex;
uniform float scale;

void main()
{
	final_color = clamp(texture(input_tex, inData.tex_coord) * scale, vec4(0.0), vec4(1.0));
}