#version 430 core

const vec2 tex_coords[3] = vec2[3] (
	vec2(0.0, 0.0),
	vec2(2.0, 0.0),
	vec2(0.0, 2.0)
);

// Output variables
out VertexData
{
	vec2 tex_coord;
} outData;

void main()
{
	vec2 tex_coord = tex_coords[gl_VertexID];

	outData.tex_coord = tex_coord;
	gl_Position = vec4(tex_coord * 2.0 - 1.0, 0.0, 1.0);
}
