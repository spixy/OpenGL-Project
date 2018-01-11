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
uniform mat4 shadow_matrix;

// Evaluates the lighting of one Phong light. The implementation is at the end of the file
void EvaluatePhongLight(in PhongLight light, out vec3 amb, out vec3 dif, out vec3 spe, in vec3 normal, in vec3 position, in vec3 eye, in float shininess);

//-----------------------------------------------------------------------

void main()
{
    // Coordinate for the shadow
    vec4 shadow_tex_coord = shadow_matrix * vec4(inData.position_ws, 1.0);
    float shadow_factor = textureProj(shadow_tex, shadow_tex_coord);

    vec3 tex_color = texture(object_tex, inData.tex_coord).rgb;

	// Compute the lighting
    vec3 N = normalize(inData.normal_ws);
    vec3 Eye = normalize(eye_position - inData.position_ws);

    vec3 a, d, s;
    EvaluatePhongLight(lights[0], a, d, s, N, inData.position_ws, Eye, material.shininess);
    vec3 amb = global_ambient_color + a;
    vec3 dif = d * shadow_factor;
    vec3 spe = s * shadow_factor;
    
	// Compute the material
    vec3 mat_ambient = tex_color;
    vec3 mat_diffuse = tex_color;
    vec3 mat_specular = material.specular;

	// Compute the final color
    vec3 final_light = mat_ambient * amb + mat_diffuse * dif + mat_specular * spe;

	//deferred_albedo = vec4(shadow_tex_coord.zzz / shadow_tex_coord.w, material.alpha);
	//deferred_albedo = vec4(vec3(shadow_factor), material.alpha);
    deferred_albedo = vec4(final_light, material.alpha);

	deferred_position_ws = vec4(inData.position_ws, 1.0);
	deferred_position_vs = vec4(inData.position_vs, 1.0);
	deferred_normal_ws = vec4(normalize(inData.normal_ws), 0.0);
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