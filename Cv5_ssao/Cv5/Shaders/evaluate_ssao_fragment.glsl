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

//-----------------------------------------------------------------------

void main()
{
	// Position and normal in view space
	vec4 position_vs = texture(position_vs_tex, inData.tex_coord);
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
			// There is no occluder (object_at_sample.w == 0.0 means the background)

			// Task 1: Evaluate the occlusion for case a), add one to occluded_test_count if this sample has a valid result,
			//				add one to occluded_samples if the sample is occluded.
			occluded_test_count += 1.0;
			occluded_samples += 0.0;
		}
		else
		{
			// We compare the distances from the camera of the occludee (the sample) and the occluder (the object)

			// Task 1: Evaluate the occlusion for cases b),c):
			//	- Compute the distances of the sample (its position is in sample_position_vs) and
			//		the possible occluder (its position is in closest_object_vs). Eye position
			//		in view space is (0,0,0).
			//		Hint: Think how the position in *view* space is related to the distance from the camera.
			//	- Compare the distances and evaluate the occlusion. Add one to occluded_test_count if this test
			//		has a valid result (it is not a valid result only in case d)), add one to occluded_samples if the sample is occluded.
			// Task 3: Evaluate the occlusion for case d):
			//	- Use SSAO_Radius to choose between c) and d) - if the occluder is closer to the viewer by more than
			//		SSAO_Radius, it is d).
			float occludee_distance = length(sample_position_vs);		// Or -sample_position_vs.z
			float occluder_distance = length(closest_object_vs.xyz);	// Or -closest_object_vs.z
			if ((occludee_distance - SSAO_Radius) >= occluder_distance)		// Result of task 3: choose between b,c) and d)
			{
				// This is d)

				// There is an occluder, but the occluder is a lot closer than the sample. The occluder object
				// may or may not be thick enough to affect the ambient occlusion, so we ignore this sample.
				//
				// In this example, we say objects have the same thickness as is the radius of the hemisphere.
				//
				// This is not a valid sample
				occluded_test_count += 0.0;
			}
			else 
			{
				// This is b,c)

				// There is an occluder, and it is not too closer. We compare the distances and decide whether
				// the sample is occluded or not.
				//
				// This is a valid sample
				occluded_test_count += 1.0;
				if (occludee_distance > occluder_distance)
					occluded_samples += 1.0;
				else
					occluded_samples += 0.0;
			}
		}
	}

	// Compute the final occlusion
	if (position_vs.w == 0.0)
		final_occlusion = 0.0;		// We processed a pixel of the background, we set its final occlusion to zero
	else if (occluded_test_count == 0.0)
		final_occlusion = 1.0;		// We found no valid sample to test, so we say there is no occlusion
	else
		final_occlusion = 1.0 - occluded_samples / occluded_test_count;		// More occluded samples -> less ambient light
}

//-----------------------------------------------------------------------