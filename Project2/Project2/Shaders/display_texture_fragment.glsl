#version 430 core

// Input variables
in VertexData
{
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out vec4 final_color;

// Texture to display and a transformation to be applied to the colors of the texture
layout (binding = 0) uniform sampler2D input_tex;
uniform mat4 transformation;

void main()
{
	// Transform the color from the texture using a matrix
	final_color = clamp(transformation * texture(input_tex, inData.tex_coord), vec4(0.0), vec4(1.0));
}