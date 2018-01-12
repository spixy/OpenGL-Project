#version 430 core

// Input variables
in VertexData
{
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out float final_blurred_ssao;

// SSAO texture to be blurred
layout (binding = 0) uniform sampler2D ssao_texture;

// Kernel for gaussian blur
const float gauss5[5] = float[5](1.0, 4.0, 6.0, 4.0, 1.0);
const float gauss5x5_sum = 16.0 * 16.0;

void main()
{
	// Gaussian blur
	vec2 texel_size = 1.0 / textureSize(ssao_texture, 0);		// Size of one texel when texture coordinates are in range (0,1)
	float sum = 0.0;
	for (int x = 0; x < gauss5.length(); x++)
	for (int y = 0; y < gauss5.length(); y++)
	{
		// Offset from the center texel
		vec2 offset = texel_size * (vec2(x, y) - vec2(gauss5.length()/2));
		// Adding weighted value
		sum += gauss5[x] * gauss5[y] * textureLod(ssao_texture, inData.tex_coord + offset, 0).r;
	}
	final_blurred_ssao = sum / gauss5x5_sum;
}