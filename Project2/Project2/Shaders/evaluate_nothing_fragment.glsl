#version 430 core

// Input variables
in VertexData
{
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out vec4 final_color;

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

// Data of the material
layout (std140, binding = 3) uniform MaterialData
{
	// See the C++ code for the documentation to individual attributes
	vec3 ambient;
	vec3 diffuse;
	float alpha;
	vec3 specular;
	float shininess;
} material;

// G-buffer textures
layout (binding = 0) uniform sampler2D positions_ws_tex;
layout (binding = 1) uniform sampler2D normals_ws_tex;
layout (binding = 2) uniform sampler2D albedo_tex;
layout (binding = 3) uniform sampler2D ssao_tex;

//-----------------------------------------------------------------------

void main()
{
	// Get the  albedo at the current pixel
	vec3 albedo = texture(albedo_tex, inData.tex_coord).xyz;

	// Compute the final color, apply ambient occlusion
    vec3 final_light = albedo;
	
	final_color = vec4(final_light, 1.0);
}