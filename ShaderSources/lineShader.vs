#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 viewProjection;
uniform int PointCount;

out float Fade; // 0 at the oldest point, 1 at the body

void main()
{
	Fade = float(gl_VertexID) / float(max(PointCount - 1, 1));
	gl_Position = viewProjection * model * vec4(aPos, 1.0);
}
