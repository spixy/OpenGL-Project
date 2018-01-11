#version 430 core

layout (location = 0) out vec4 final_color;

in VertexData
{
	vec2 tex_coord;
} inData;

// Data of the object
uniform int blur_kernel_size;
uniform float gauss_array[128];
uniform float gauss_sum;

layout (binding = 0) uniform sampler2D tex;

void main()
{
	// Gaussian blur
	vec2 texel_size = 1.0 / textureSize(tex, 0);
	vec3 sum = vec3(0.0, 0.0, 0.0);
	int offset_half = blur_kernel_size / 2;

	for (int y = 0; y < blur_kernel_size; ++y)
	{
		// Offset from the center texel
		vec2 offset = vec2(0.0, texel_size.y * (y - offset_half));
		// Adding weighted value
		sum += gauss_array[y] * textureLod(tex, inData.tex_coord + offset, 0).xyz;
	}
	final_color = vec4(sum / gauss_sum, 0.0);
}