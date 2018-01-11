#version 430 core

#define LIGHTS_MAX_COUNT			2000
#define LIGHTS_STORAGE				buffer		// Choose between 'uniform' and 'buffer'
const float light_range = 8.0f;

// Input variables
layout (location = 0) in vec4 position;

// Output variables
out VertexData
{
	noperspective vec2 tex_coord;		// Texture coordinate for sampling G-buffer textures
	flat int light_idx;					// Index of the light that is being processed
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
layout (std140, binding = 1) LIGHTS_STORAGE PhongLightsData
{
	vec3 global_ambient_color;
	int lights_count;
	PhongLight lights[LIGHTS_MAX_COUNT];
};

//-----------------------------------------------------------------------

void main()
{
	// Task 3: Transform the sphere, transform it into the clip space (i.e. transform it also by the view and
	//			projection matrices), store the position into pos_cs.
	//			Hint: position.xyz is the position of the vertex of the unit sphere we render, lights[...].position.xyz
	//				is the position of the light, light_range is the range into which the light lights.
	//				gl_InstanceID is the index the current instance, which is also the index of the light that is processed.
	// Position in world space
	vec3 pos_ws = position.xyz * light_range + lights[gl_InstanceID].position.xyz;
	// Position in clip space
	vec4 pos_cs = projection * view * vec4(pos_ws, 1.0);

	outData.light_idx = gl_InstanceID;
	outData.tex_coord = pos_cs.xy / pos_cs.w * 0.5 + 0.5;
	gl_Position = pos_cs;
}
