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
	vec2 half_size = vec2(blur_kernel_size / 2);
	vec3 sum = vec3(0.0, 0.0, 0.0);
	for (int x = 0; x < blur_kernel_size; ++x)
	for (int y = 0; y < blur_kernel_size; ++y)
	{
		// Offset from the center texel
		vec2 offset = texel_size * (vec2(x, y) - half_size);
		// Adding weighted value
		sum += (gauss_array[x] * gauss_array[y]) * textureLod(tex, inData.tex_coord + offset, 0).xyz;
	}
	final_color = vec4(sum / gauss_sum, 0.0);
}