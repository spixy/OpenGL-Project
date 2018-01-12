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

layout (binding = 0) uniform sampler2DShadow shadow_tex;
uniform mat4 shadow_matrix;

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

//-----------------------------------------------------------------------

void main()
{
    /*
	// Coordinate for the shadow
	vec4 shadow_tex_coord = shadow_matrix * vec4(inData.position_ws, 1.0);
	//float shadow_factor = texture(shadow_tex, shadow_tex_coord.xy / shadow_tex_coord.w).r;
	//float shadow_factor = (shadow_tex_coord.z / shadow_tex_coord.w) < texture(shadow_tex, shadow_tex_coord.xy / shadow_tex_coord.w).r ? 1.0 : 0.0;
	//float shadow_factor = texture(shadow_tex, shadow_tex_coord.xyz / shadow_tex_coord.w);
	float shadow_factor = textureProj(shadow_tex, shadow_tex_coord);

	// Compute the lighting
    vec3 N = normalize(inData.normal_ws);
    vec3 Eye = normalize(eye_position - inData.position_ws);

    vec3 amb = global_ambient_color;
    vec3 dif = vec3(0.0);
    vec3 spe = vec3(0.0);

    vec3 a, d, s;
    EvaluatePhongLight(lights[0], a, d, s, N, inData.position_ws, Eye, material.shininess);
    amb += a;
    dif += d;
    spe += s;
    
	d *= shadow_factor;
	s *= shadow_factor;

	// Compute the material
    vec3 mat_ambient = material.ambient;
    vec3 mat_diffuse = material.diffuse;
    vec3 mat_specular = material.specular;

	// Compute the final color
    vec3 final_light = mat_ambient * amb + mat_diffuse * dif + mat_specular * spe;
    deferred_albedo = vec4(final_light, material.alpha);
    */
    deferred_albedo = vec4(material.diffuse, material.alpha);
    deferred_position_ws = vec4(inData.position_ws, 1.0);
    deferred_position_vs = vec4(inData.position_vs, 1.0);
    deferred_normal_ws = vec4(normalize(inData.normal_ws), 0.0f);
    deferred_normal_vs = vec4(normalize(inData.normal_vs), 0.0);
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
        else
            spot_factor = 0.0;

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