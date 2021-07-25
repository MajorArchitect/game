#version 330 core
out vec4 FragColour;

in vec3 v_pos;
/* Used to determine the texture co-ords*/
in vec2 v_tex;
in vec3 v_col;

uniform sampler2D tex_sam1;

void main()
{
	FragColour = texture(tex_sam1, v_tex);
}
