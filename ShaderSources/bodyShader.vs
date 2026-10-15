#version 330 core
layout (location = 0) in vec3 aPolar;

uniform vec3 worldSpherical;

uniform mat4 model;
uniform mat4 viewProjection;

vec3 sphericalToCartesian(vec3 spherical){
	float r = spherical.x;
	float theta = spherical.y;
	float phi = spherical.z;

	float x = r * sin(theta)*cos(phi); 
	float y = r*cos(theta); // in screen coords y is up
	float z = r*sin(theta)*sin(phi);

	return vec3(x,y,z);
}

void main()
{
	vec3 localCartesianPos = sphericalToCartesian(aPolar);

	gl_Position = viewProjection * model * vec4(localCartesianPos, 1.0);
}