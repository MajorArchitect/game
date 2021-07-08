#version 330 core
out vec4 FragColour;

in vec3 v_pos;
in vec3 v_col;
in vec2 v_tex;

uniform sampler2D tex_sam1;

void main()
{
	FragColour = mix(texture(tex_sam1, v_tex),  vec4(v_col, 1.0f), 0);
	/*FragColour = vec4((v_pos.x / 2.0) + 0.5, (v_pos.y / 2.0) + 0.5,
		(v_pos.z / 2.0) + 0.5, 1.0f);*/
}
