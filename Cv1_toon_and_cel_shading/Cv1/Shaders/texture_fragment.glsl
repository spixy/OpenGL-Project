#version 430 core

// Input variables
in VertexData
{
	vec3 normal_ws;
	vec3 position_ws;
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

// Data of the object
layout (binding = 0) uniform sampler2D object_tex;

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
	vec3 tex_color = texture(object_tex, inData.tex_coord).rgb;

	// Task 4: Threshold tex_color, each channel (R, G, B) to four intensities.
	//			Hint: think about functions mix, step, and smoothstep, they compute
	//			all three components at the same time.
	tex_color = mix(vec3(0.2), mix(vec3(0.5), mix(vec3(0.8), vec3(1.0),
		smoothstep(0.75, 0.85, tex_color)), smoothstep(0.55, 0.65, tex_color)), smoothstep(0.35, 0.45, tex_color));

	// Compute the lighting
	vec3 N = normalize(inData.normal_ws);
	vec3 Eye = normalize(eye_position - inData.position_ws);

	vec3 amb = global_ambient_color;
	vec3 dif = vec3(0.0);
	vec3 spe = vec3(0.0);

	for (int i = 0; i < lights_count; i++)
	{
		// Task 3: Implement toon-shading for diffuse lighting and specular lighting similarly
		//			as in notexture_fragment.glsl
		vec3 a, d, s;
		EvaluatePhongLight(lights[i], a, d, s, N, inData.position_ws, Eye, material.shininess);

		if (d.x < 0.4)				d = vec3(0.2);
		else if (d.x < 0.6)			d = vec3(0.5);
		else if (d.x < 0.8)			d = vec3(0.8);
		else						d = vec3(1.0);

		if (s.x < 0.6)				s = vec3(0.0);
		else						s = vec3(1.0);

		amb += a;	dif += d;	spe += s;
	}

	// Compute the material
	vec3 mat_ambient = tex_color;
	vec3 mat_diffuse = tex_color;
	vec3 mat_specular = material.specular;

	// Compute the final color
	vec3 final_light = mat_ambient * amb + mat_diffuse * dif + mat_specular * spe;

	// Task 5a: Use N and Eye and compute the contour
	//float edge = step(0.2, dot(N, Eye));
	//final_color = vec4(final_light*edge, 1.0);

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