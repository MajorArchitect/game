#version 330 core
layout (location = 0) in vec3 a_pos;

out vec3 v_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(a_pos, 1.0f);
	v_pos = a_pos;
}
