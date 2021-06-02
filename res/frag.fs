#version 330 core
out vec4 FragColour;

in vec3 v_col;
in vec2 v_tex;

uniform sampler2D tex_sam1;
uniform sampler2D tex_sam2;

void main()
{
	FragColour = mix(texture(tex_sam1, v_tex),  texture(tex_sam2, v_tex),
		0);
}
