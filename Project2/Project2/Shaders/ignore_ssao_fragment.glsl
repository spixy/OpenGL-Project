#version 430 core

// Input variables
in VertexData
{
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out float final_occlusion;

// Data of the camera
layout (std140, binding = 0) uniform CameraData
{
	mat4 projection;		// Projection matrix
	mat4 projection_inv;	// Inverse of the projection matrix
	mat4 view;				// View matrix
	mat4 view_inv;			// Inverse of the view matrix
	mat3 view_it;			// Inverse of the transpose of the top-left part 3x3 of the view matrix
	vec3 eye_position;		// Position of the eye in world space
};

// Random positions of the SSAO kernel
layout (std140, binding = 1) uniform KernelSamples
{
	vec4 kernel_samples[64];
};

// Input data for the SSAO - positions, normals and random tangent (in view space)
layout (binding = 0) uniform sampler2D position_vs_tex;
layout (binding = 1) uniform sampler2D normal_vs_tex;
layout (binding = 2) uniform sampler2D random_tangent_vs_tex;

//-----------------------------------------------------------------------

void main()
{
	final_occlusion = 1.0;
}

//-----------------------------------------------------------------------