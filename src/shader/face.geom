#version 330 core

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

in V2G
{
 vec4 position; 
 vec3 normal;
 vec3 albedo;
} vertices[3];

out G2P
{
	vec3 normal;
	vec3 albedo;
	vec3 barycentrics; 
} v;

void main()
{
	gl_Position = vertices[0].position; 
	v.normal = vertices[0].normal;
	v.albedo = vertices[0].albedo; 
	v.barycentrics = vec3(1,0,0); 
	EmitVertex();

	gl_Position = vertices[1].position;
	v.normal = vertices[1].normal;
	v.albedo = vertices[1].albedo; 
	v.barycentrics = vec3(0,1,0); 
	EmitVertex();

	gl_Position = vertices[2].position;
	v.normal = vertices[2].normal;
	v.albedo = vertices[2].albedo; 
	v.barycentrics = vec3(0,0,1); 
	EmitVertex();
}