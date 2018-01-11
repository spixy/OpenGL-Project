#version 430 core

// Input variables
in VertexData
{
	vec3 normal_ws;
	vec3 position_ws;
} inData;

// Output variables
layout (location = 0) out vec4 deferred_position;
layout (location = 1) out vec4 deferred_normal;
layout (location = 2) out vec4 deferred_albedo;

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

//-----------------------------------------------------------------------

void main()
{
	// Task 1: Compute deferred_position, deferred_normal, and deferred_albedo. Use diffuse color as albedo.
	//			Hint: Look into notexture_forward_fragment.glsl to see what to use.
	deferred_position = vec4(inData.position_ws, 1.0);
	deferred_normal = vec4(normalize(inData.normal_ws), 0.0f);
	deferred_albedo = vec4(material.diffuse, 1.0);
}

//-----------------------------------------------------------------------