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

// Evaluates the lighting of one Phong light. The implementation is at the end of the file
void EvaluatePhongLight(in PhongLight light, out vec3 amb, out vec3 dif, out vec3 spe, in vec3 normal, in vec3 position, in vec3 eye, in float shininess);

// G-buffer textures
layout (binding = 0) uniform sampler2D positions_ws_tex;
layout (binding = 1) uniform sampler2D normals_ws_tex;
layout (binding = 2) uniform sampler2D albedo_tex;
layout (binding = 3) uniform sampler2D ssao_tex;
layout (binding = 1) uniform sampler2DShadow shadow_tex;

uniform mat4 shadow_matrix;

//-----------------------------------------------------------------------

void main()
{
	// Get the position, normal, albedo and SSAO at the current pixel
	vec3 position_ws = texture(positions_ws_tex, inData.tex_coord).xyz;
	vec3 normal_ws = normalize(texture(normals_ws_tex, inData.tex_coord).xyz);
	vec3 albedo = texture(albedo_tex, inData.tex_coord).xyz;
    float ssao = texture(ssao_tex, inData.tex_coord).r;
    
	// Coordinate for the shadow
    //vec4 shadow_tex_coord = shadow_matrix * vec4(position_ws, 1.0);
    float shadow_factor = 1.0; //textureProj(shadow_tex, shadow_tex_coord);

	// Compute the lighting
	vec3 N = normal_ws;
	vec3 Eye = normalize(eye_position - position_ws);

	vec3 amb = global_ambient_color;
	vec3 dif = vec3(0.0);
	vec3 spe = vec3(0.0);

	for (int i = 0; i < lights_count; i++)
	{
		vec3 a, d, s;
		EvaluatePhongLight(lights[i], a, d, s, N, position_ws, Eye, material.shininess);
		amb += a;	dif += d * shadow_factor;	spe += s * shadow_factor;
	}

	// Compute the final color, apply ambient occlusion
	vec3 final_light = albedo * amb * ssao + albedo * dif + material.specular * spe;
	
	final_color = vec4(final_light, 1.0);
}

//-----------------------------------------------------------------------

// Evaluates the lighting of one Phong light
// light	.. [in] parameters of the light that is evaluated
// amb		.. [out] result, ambient part
// dif		.. [out] result, diffuse part
// spe		.. [out] result, specular part
// norm		.. [in] surface normal, in the world coordinates
// pos		.. [in] surface position, in the world coordinates
// eye		.. [in] direction from the surface to the eye, in the world coordinates
// shi		.. [in] shininess of the material
void EvaluatePhongLight(in PhongLight light, out vec3 amb, out vec3 dif, out vec3 spe, in vec3 norm, in vec3 pos, in vec3 eye, in float shi)
{
	vec3 L_not_normalized = light.position.xyz - pos * light.position.w;
	vec3 L = normalize(L_not_normalized);
	vec3 H = normalize(L + eye);

	// Calculate basic Phong factors
	float Iamb = 1.0;
	float Idif = max(dot(norm, L), 0.0);
	float Ispe = (Idif > 0.0) ? pow(max(dot(norm, H), 0.0), shi) : 0.0;

	// Calculate spot light factor
	if (light.spot_cos_cutoff != -1.0)
	{
		// This is a spot light
		float spot_factor;

		float spot_cos_angle = dot(-L, light.spot_direction);
		if (spot_cos_angle > light.spot_cos_cutoff)
		{
			spot_factor = pow(spot_cos_angle, light.spot_exponent);
		}
		else spot_factor = 0.0;

		Iamb *= 1.0;
		Idif *= spot_factor;
		Ispe *= spot_factor;
	}

	// Calculate attenuation
	if (light.position.w != 0.0)
	{
		// This is a point light / spot light

		float distance_from_light = length(L_not_normalized);
		float atten_factor =
			light.atten_constant +
			light.atten_linear * distance_from_light + 
			light.atten_quadratic * distance_from_light * distance_from_light;
		atten_factor = 1.0 / atten_factor;

		Iamb *= atten_factor;
		Idif *= atten_factor;
		Ispe *= atten_factor;
	}

	amb = Iamb * light.ambient;
	dif = Idif * light.diffuse;
	spe = Ispe * light.specular;
}