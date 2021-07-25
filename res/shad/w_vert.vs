#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in int a_texref;
layout (location = 2) in int a_type;

out vec3 v_pos;
out vec2 v_tex;
out vec3 v_col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	if (a_texref == 0) {
		v_tex = vec2(0.5, 0.5);
		v_col = vec3(1.0, 1.0, 1.0);
	} else if (a_texref == 1) {
		v_tex = vec2(0.0, 1.0);
		v_col = vec3(1.0, 0.0, 0.0);
	} else if (a_texref == 2) {
		v_tex = vec2(0.0, 0.0);
		v_col = vec3(1.0, 1.0, 0.0);
	} else if (a_texref == 3) {
		v_tex = vec2(1.0, 0.0);
		v_col = vec3(0.0, 1.0, 0.0);
	} else if (a_texref == 4) {
		v_tex = vec2(1.0, 1.0);
		v_col = vec3(0.0, 0.0, 1.0);
	}
	v_tex.x /= 16.0;
	v_tex.y /= 16.0;

	v_tex.x += (a_type % 16) / 16.0;
	v_tex.y += (a_type / 16) / 16.0;

	v_tex.y = 1.0 - v_tex.y;

	gl_Position = proj * view * model * vec4(a_pos, 1.0f);
	v_pos = a_pos;


}
