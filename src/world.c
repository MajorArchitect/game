#include "world.h"
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

//GLOBAL VARIABLE DECLARATIONS
short heightmap[256][256] = {{0}, {0, 20, 20, 20, 20, 0},
	{0, 20, 40, 40, 20, 0}, {0, 20, 40, 40, 20, 0}, {0, 20, 20, 20, 20, 0}};

struct s_tiledat {
	char type;
};
struct s_tiledat tiledat[256][256] = {{{1}, {1}, {1}, {1}},
	{{1}, {2}, {2}, {1}}, {{1}, {2}, {2}, {1}}, {{1}, {1}, {1}, {1}}};


//256 tiles is the render distance for now, but this should be
//made into a #define.
vec3 c_vec[256][256];
vec3 t_vec[256][256];

struct vbo_vert {
	vec3 vert;
	unsigned char texref;
	char tiletype;
};

unsigned int grass_tex = 0;

unsigned int world_vbo = 0, world_vao = 0, world_shader = 0;



void makeworld()
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			heightmap[i][j] =  (int)(400.0f *
				perlin((vec2){{(float)j*4.0f-512.0f, (float)i*4.0f-512.0f}}));
		}
	}

	//Make the buffers exist.
	glGenVertexArrays(1, &world_vao);
	glBindVertexArray(world_vao);
	glGenBuffers(1, &world_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, world_vbo);

	//Assembling the grid of vectors to be turned into a triangle strip
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			/* What this means:
			 * One tile is a 4m by 4m square.
			 * One height unit is 0.1m.
			 * The centre point of each tile is the average
			 * height of all its corners. */
			c_vec[i][j].e[0] = (float)((j - 128) * 4);
			c_vec[i][j].e[1] = (float)((i - 128) * 4);
			c_vec[i][j].e[2] = (float)(heightmap[i][j] * 0.1f);
		}
	}
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			t_vec[i][j].e[0] = (float)(((j - 128) * 4) + 2);
			t_vec[i][j].e[1] = (float)(((i - 128) * 4) + 2);
			t_vec[i][j].e[2] =
			(float)((c_vec[i][j].e[2] + c_vec[i + 1][j].e[2]
			+ c_vec[i][j + 1].e[2] + c_vec[i + 1][j + 1].e[2])
			/ 4.0f);
		}
	}
	//Assembling the triangle fans
	struct vbo_vert tri_fan[256][1536] = {0};
	for (int i = 0; i < 255; i++) {
		for (int j = 0; j < 255; j++) {
			int down1, right1;


			int j_6 = (j * 6);

			tri_fan[i][j_6 + 0]  =
				(struct vbo_vert){t_vec[i][j], 0,
					tiledat[i][j].type};
			tri_fan[i][j_6 + 1]  =
				(struct vbo_vert){c_vec[i][j], 1,
					tiledat[i][j].type};
			tri_fan[i][j_6 + 2]  =
				(struct vbo_vert){c_vec[i + 1][j], 2,
					tiledat[i][j].type};
			tri_fan[i][j_6 + 3]  =
				(struct vbo_vert){c_vec[i + 1][j + 1], 3,
					tiledat[i][j].type};
			tri_fan[i][j_6 + 4]  =
				(struct vbo_vert){c_vec[i][j + 1], 4,
					tiledat[i][j].type};
			tri_fan[i][j_6 + 5]  =
				(struct vbo_vert){c_vec[i][j], 1,
					tiledat[i][j].type};
		}
	}

	//Put the strip in the GL
	glBufferData(GL_ARRAY_BUFFER, 1536 * 256 * (sizeof(struct vbo_vert)),
		tri_fan, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(struct vbo_vert), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE,
		sizeof(struct vbo_vert), (void *)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE,
		sizeof(struct vbo_vert), (void *)(sizeof(vec3)) + sizeof(char));
	glEnableVertexAttribArray(2);

	//Make the shader
	world_shader = mkshader("res/shad/w_vert.vs",
		"res/shad/w_frag.fs");
	if (world_shader == 0) {
		printf("Couldn't make world shader, exiting.\n");
		exit(1);
	}
	//Add the texture
	glGenTextures(1, &grass_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_tex);
	//Set its parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Read the file
	int tex_width, tex_height, tex_chans;
	stbi_set_flip_vertically_on_load(1);
	//fileread function
	unsigned char *data = stbi_load("res/tex/tiles.png", &tex_width, &tex_height,
		&tex_chans, 0);
	if (data == NULL) {
		printf("image data is NULL\n");
		//return -2;
	}
	//Set its data, depending on the depth of the pixels
	if (tex_chans == 3)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);
	else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(world_shader, "tex_sam1"), 0);

	glBindVertexArray(0);
}

void drawworld(mat4 view, mat4 proj)
{
	//This runs the first time the function is run.
	if (world_vbo == 0) { //If one of them is 0, both of them should be.
		makeworld();
	}

	//Bind and use
	glBindVertexArray(world_vao);
	glUseProgram(world_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_tex);

	mat4 model = mat_ident(1.0f);

	unsigned int model_loc = glGetUniformLocation(world_shader, "model");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, (float *)model.e);
	unsigned int view_loc = glGetUniformLocation(world_shader, "view");
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, (float *)view.e);
	unsigned int proj_loc = glGetUniformLocation(world_shader, "proj");
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, (float *)proj.e);

	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			glDrawArrays(GL_TRIANGLE_FAN, (j * 1536) + i * 6, 6);
		}
	}

	glBindVertexArray(0);
	return;

}

vec2 gradvec[5][5] = {0};
float perlin(vec2 coords)
{
	//Normalise the world co-ordinates to 0.0 -> 4.0
	vec2 sclcoords = vec2_scale(coords, 1.0f / 256.0f);
	sclcoords = vec2_add(sclcoords, (vec2){{2.0f, 2.0f}});

	//Determine which cell coords are in
	int cellx = (int)floorf(sclcoords.e[0]);
	int celly = (int)floorf(sclcoords.e[1]);
	float cellxf = (float)cellx;
	float cellyf = (float)celly;

	//Find the offset vectors
	vec2 offvec[2][2];
	offvec[0][0] = vec2_sub((vec2){{cellxf, cellyf}}, sclcoords);
	offvec[1][0] = vec2_sub((vec2){{cellxf+1.0f, cellyf}}, sclcoords);
	offvec[0][1] = vec2_sub((vec2){{cellxf, cellyf+1.0f}}, sclcoords);
	offvec[1][1] = vec2_sub((vec2){{cellxf+1.0f, cellyf+1.0f}}, sclcoords);

	//Get dot products between offset vectors and gradient vectors
	float dotprod[2][2];
	dotprod[0][0] = vec2_dot(offvec[0][0], gradvec[celly][cellx]);
	dotprod[1][0] = vec2_dot(offvec[1][0], gradvec[celly+1][cellx]);
	dotprod[0][1] = vec2_dot(offvec[0][1], gradvec[celly][cellx=1]);
	dotprod[1][1] = vec2_dot(offvec[1][1], gradvec[celly+1][cellx+1]);

	//bilerp all the dot products to the point
	float lerp1[2];
	lerp1[0] = dotprod[0][0] + (sclcoords.e[0] - cellxf)
		* (dotprod[0][1] - dotprod[0][0]);
	lerp1[0] = dotprod[1][0] + (sclcoords.e[0] - cellxf)
		* (dotprod[1][1] - dotprod[1][0]);

	float lerp2 = lerp1[0] + (sclcoords.e[1] - cellyf)
		* (lerp1[1] - lerp1[0]);

	//return the bilerp.
	return lerp2;
}

void seedperlin(unsigned int seed)
{
	srand(seed);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			float angle = (float)rand()/(float)(RAND_MAX/ (2.0f * 3.14159));
			mat4 rot = mat_rot(mat_ident(1.0f), angle,
				(vec3){{0.0f, 0.0f, 1.0f}});
			vec4 bigvec = matvecprod(rot,
				(vec4){{1.0f, 0.0f, 0.0f, 1.0f}});
			gradvec[i][j] = (vec2){{bigvec.e[0], bigvec.e[1]}};
		}
	}
}


void freeworld()
{
	glDeleteVertexArrays(1, &world_vao);
	glDeleteBuffers(1, &world_vbo);

	return;
}

float terrainheight(vec2 coords)
{
	float x_dist = (coords.e[0] / 4.0f) - floorf(coords.e[0] / 4.0f);
	float y_dist = (coords.e[1] / 4.0f) - floorf(coords.e[1] / 4.0f);
	float h = sqrtf(x_dist * x_dist + y_dist * y_dist);
	float root2 = sqrtf(2.0f) / 2.0f;

	vec3 p1, p2, p3;
	p1 = t_vec[(int)floorf(coords.e[1] / 4.0f) + 128]
		[(int)floorf(coords.e[0] / 4.0f) + 128];
	if (h >= root2) {
		p2 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 129]
			[(int)floorf(coords.e[0] / 4.0f) + 129];
		if (y_dist >= x_dist) {
			p3 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 129]
				[(int)floorf(coords.e[0] / 4.0f) + 128];
		} else {
			p3 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 128]
				[(int)floorf(coords.e[0] / 4.0f) + 129];
		}
	} else {
		p3 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 128]
			[(int)floorf(coords.e[0] / 4.0f) + 128];
		if (y_dist >= x_dist) {
			p2 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 129]
				[(int)floorf(coords.e[0] / 4.0f) + 128];
		} else {
			p2 = c_vec[(int)floorf(coords.e[1] / 4.0f) + 128]
				[(int)floorf(coords.e[0] / 4.0f) + 129];
		}
	}

	float det = (p2.e[1] - p3.e[1]) * (p1.e[0] - p3.e[0]) +
		(p3.e[0] - p2.e[0]) * (p1.e[1] - p3.e[1]);

	float l1 = ((p2.e[1] - p3.e[1]) * (coords.e[0] - p3.e[0])
		+ (p3.e[0] - p2.e[0]) * (coords.e[1] - p3.e[1])) / det;
	float l2 = ((p3.e[1] - p1.e[1]) * (coords.e[0] - p3.e[0])
		+ (p1.e[0] - p3.e[0]) * (coords.e[1] - p3.e[1])) / det;
	float l3 = 1.0f - l1 - l2;

	return l1 * p1.e[2] + l2 * p2.e[2] + l3 * p3.e[2];
}
