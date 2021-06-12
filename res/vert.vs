#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_col;
layout (location = 2) in vec2 a_tex;

out vec3 v_pos;
out vec3 v_col;
out vec2 v_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(a_pos, 1.0f);
	v_pos = a_pos;
	v_col = a_col;
	v_tex = a_tex;
}
