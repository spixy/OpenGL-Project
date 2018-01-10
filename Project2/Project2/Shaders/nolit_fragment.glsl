#version 430 core

// Input variables - no input variables

// Output variables
layout (location = 0) out vec4 final_color;

// Data of the material
layout (std140, binding = 3) uniform MaterialData
{
	vec3 ambient;
	vec3 diffuse;
	float alpha;
	vec3 specular;
	float shininess;
} material;

//-----------------------------------------------------------------------

void main()
{
	final_color = vec4(material.diffuse, material.alpha);
}

//-----------------------------------------------------------------------