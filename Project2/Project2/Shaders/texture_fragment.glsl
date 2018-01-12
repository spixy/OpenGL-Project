#version 430 core

// Input variables
in VertexData
{
	vec3 position_ws;
	vec3 position_vs;
	vec3 normal_ws;
	vec3 normal_vs;
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out vec4 deferred_position_ws;
layout (location = 1) out vec4 deferred_position_vs;
layout (location = 2) out vec4 deferred_normal_ws;
layout (location = 3) out vec4 deferred_normal_vs;
layout (location = 4) out vec4 deferred_albedo;

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

// Data of the light
struct PhongLight
{
	// See the C++ code for the documentation to individual attributes
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 spot_direction;
	float spot_exponent;
	float spot_cos_cutoff;
	float atten_constant;
	float atten_linear;
	float atten_quadratic;
};
layout (std140, binding = 1) uniform PhongLightsData
{
	vec3 global_ambient_color;
	int lights_count;
	PhongLight lights[8];
};

// Data of the object
layout (binding = 0) uniform sampler2DShadow shadow_tex;
layout (binding = 1) uniform sampler2D object_tex;

//-----------------------------------------------------------------------

void main()
{
    // shadow by sa dal pocitat aj tu
    deferred_albedo = texture(object_tex, inData.tex_coord);
	deferred_position_ws = vec4(inData.position_ws, 1.0);
	deferred_position_vs = vec4(inData.position_vs, 1.0);
	deferred_normal_ws = vec4(normalize(inData.normal_ws), 0.0);
	deferred_normal_vs = vec4(normalize(inData.normal_vs), 0.0);
}

//-----------------------------------------------------------------------