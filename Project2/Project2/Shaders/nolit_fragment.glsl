#version 430 core

// Input variables
in VertexData
{
	vec3 position_ws;
	vec3 position_vs;
	vec3 normal_ws;
	vec3 normal_vs;
} inData;

// Output variables
layout (location = 0) out vec4 deferred_position_ws;
layout (location = 1) out vec4 deferred_position_vs;
layout (location = 2) out vec4 deferred_normal_ws;
layout (location = 3) out vec4 deferred_normal_vs;
layout (location = 4) out vec4 deferred_albedo;


// Data of the material
layout (std140, binding = 3) uniform MaterialData
{
	vec3 ambient;
	vec3 diffuse;
	float alpha;
	vec3 specular;
	float shininess;
} material;

//-----------------------------------------------------------------------

void main()
{
    deferred_albedo = vec4(material.diffuse, material.alpha);
    deferred_position_ws = vec4(inData.position_ws, 1.0);
    deferred_position_vs = vec4(inData.position_vs, 1.0);
    deferred_normal_ws = vec4(normalize(inData.normal_ws), 0.0f);
    deferred_normal_vs = vec4(normalize(inData.normal_vs), 0.0);
}

//-----------------------------------------------------------------------