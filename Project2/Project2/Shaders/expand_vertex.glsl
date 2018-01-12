#version 430 core

// Input variables
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

// Output variables
out VertexData
{
	vec3 position_ws;	// Position in world space
	vec3 position_vs;	// Position in view space
	vec3 normal_ws;		// Normal in world space
	vec3 normal_vs;		// Normal in view space
} outData;

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

// Data of the object
layout (std140, binding = 2) uniform ModelData
{
	mat4 model;			// Model matrix
	mat4 model_inv;		// Inverse of the model matrix
	mat3 model_it;		// Inverse of the transpose of the top-left part 3x3 of the model matrix
};

//-----------------------------------------------------------------------

void main()
{
	vec4 pos = position + vec4(normal, 0.0) * 0.015;
	
	outData.position_ws = vec3(model * pos);
	outData.position_vs = vec3(view * model * pos);
	outData.normal_ws = normalize(model_it * normal);
	outData.normal_vs = normalize(view_it * model_it * normal);

	gl_Position = projection * view * model * pos;
}
