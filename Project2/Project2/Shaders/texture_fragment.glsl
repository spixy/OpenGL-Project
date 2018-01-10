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

// Data of the object
layout (binding = 0) uniform sampler2D object_tex;

//-----------------------------------------------------------------------

void main()
{
	deferred_position_ws = vec4(inData.position_ws, 1.0);
	deferred_position_vs = vec4(inData.position_vs, 1.0);
	deferred_normal_ws = vec4(normalize(inData.normal_ws), 0.0);
	deferred_normal_vs = vec4(normalize(inData.normal_vs), 0.0);
	deferred_albedo = texture(object_tex, inData.tex_coord);
}

//-----------------------------------------------------------------------