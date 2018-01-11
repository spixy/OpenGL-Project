#version 430 core

// Input variables
in VertexData
{
	vec3 normal_ws;
	vec3 position_ws;
	vec2 tex_coord;
} inData;

// Output variables
layout (location = 0) out vec4 deferred_position;
layout (location = 1) out vec4 deferred_normal;
layout (location = 2) out vec4 deferred_albedo;

// Data of the object
layout (binding = 0) uniform sampler2D object_tex;

//-----------------------------------------------------------------------

void main()
{
	// Task 1: Compute deferred_position, deferred_normal, and deferred_albedo. Use diffuse color as albedo.
	//			Hint: Look into texture_forward_fragment.glsl to see what to use.
	deferred_position = vec4(inData.position_ws, 1.0);
	deferred_normal = vec4(normalize(inData.normal_ws), 0.0f);
	deferred_albedo = texture(object_tex, inData.tex_coord);
}

//-----------------------------------------------------------------------