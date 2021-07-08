#version 330 core
out vec4 FragColour;

in vec3 v_pos;

//uniform sampler2D tex_sam1;

void main()
{
	FragColour = vec4(0.0f, 0.8f, 0.0f, 1.0f);
	/*FragColour = vec4((v_pos.x / 2.0) + 0.5, (v_pos.y / 2.0) + 0.5,
		(v_pos.z / 2.0) + 0.5, 1.0f);*/
}
