#version 430 core

// Input variables
in VertexData
{
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out float final_occlusion;

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

// Random positions of the SSAO kernel
layout (std140, binding = 1) uniform KernelSamples
{
	vec4 kernel_samples[64];
};

// Input data for the SSAO - positions, normals and random tangent (in view space)
layout (binding = 0) uniform sampler2D position_vs_tex;
layout (binding = 1) uniform sampler2D normal_vs_tex;
layout (binding = 2) uniform sampler2D random_tangent_vs_tex;

// Radius of the SSAO hemisphere which is sampled
uniform float SSAO_Radius;

float compute_ssao();

//-----------------------------------------------------------------------

void main()
{
	final_occlusion = compute_ssao();
}

//-----------------------------------------------------------------------

float compute_ssao()
{
	// Position and normal in view space
	vec4 position_vs = texture(position_vs_tex, inData.tex_coord);

	if (position_vs.w == 0.0)
		return 0.0;		// We processed a pixel of the background, we set its final occlusion to zero

	vec3 normal_vs = normalize(texture(normal_vs_tex, inData.tex_coord).xyz);

	// Compute tangent and bitangent (in view space), randomly rotated around z-axis.
	// gl_FragCoord.xy contains position of the fragment in window space, i.e., its values are
	// from (viewport.x, viewport.y) to (viewport.x + viewport.width, viewport.y + viewport.height).
	// We use this fragment position as a texture coordinate and sample random tangent texture.
	//
	// We get a tangent that is not necessarly perpendicular to the normal (not yet)
	vec3 tangent_vs = texture(random_tangent_vs_tex, gl_FragCoord.xy / vec2(textureSize(random_tangent_vs_tex, 0))).xyz;
	// Compute the bitangent so that it is perpendicular to both the tangent and the normal
	vec3 bitangent_vs = normalize(cross(normal_vs, tangent_vs));
	// Recompute the tangent so that it is perpendicular to both the normal and the bitangent
	tangent_vs = normalize(cross(bitangent_vs, normal_vs));
	// Create a 3x3 matrix from the tangent, bitangent, and normal.
	//		TBN * V.xyz == tangent * V.x + bitangent * V.y + normal * V.z
	mat3 TBN = mat3(tangent_vs, bitangent_vs, normal_vs);

	// Number of samples that are occluded
	float occluded_samples = 0.0f;
	// Number of samples that we tested for occlusion
	float occluded_test_count = 0.0f;
	for (int i = 0; i < 64; i++)
	{
		// Compute the position of the sample in view space
		vec3 sample_offset_vs = TBN * kernel_samples[i].xyz * SSAO_Radius;
		vec3 sample_position_vs = position_vs.xyz + sample_offset_vs;
		// Transform the position into the clip space ...
		vec4 sample_position_cs = projection * vec4(sample_position_vs, 1.0);
		// ... and then into normalized device coordinates, ...
		vec3 sample_position_nds = sample_position_cs.xyz / sample_position_cs.w;
		// ... so that we can obtain the position the closest object at that sample (in view space)
		vec4 closest_object_vs = textureLod(position_vs_tex, sample_position_nds.xy * 0.5 + 0.5, 0);

		// Compare the distance
		if (closest_object_vs.w == 0.0)
		{
			occluded_test_count += 1.0;
			//occluded_samples += 0.0;
		}
		else
		{
			float occludee_distance = length(sample_position_vs);		// Or -sample_position_vs.z
			float occluder_distance = length(closest_object_vs.xyz);	// Or -closest_object_vs.z
            if ((occludee_distance - SSAO_Radius) >= occluder_distance)		// Choose between b), c) and d)
            {
				//occluded_test_count += 0.0;
            }
            else
            {
                occluded_test_count += 1.0;

                if (occludee_distance > occluder_distance)
                    occluded_samples += 1.0;
				/*else
					occluded_samples += 0.0;*/
            }
        }
	}

	// Compute the final occlusion
	if (occluded_test_count == 0.0)
		return 1.0;		// We found no valid sample to test, so we say there is no occlusion
	else
		return 1.0 - occluded_samples / occluded_test_count;		// More occluded samples -> less ambient light
}
