#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "entity.h"
#include "globals.h"
#include "matvec.h"



//Increments every time a new id is allocated.
int idcounter = 1;

//Mesh loading return struct.
struct meshld_ret {
	vertex *vert;
	unsigned int *index;
	ivec2 count; //count[0] is vertex count and count[1] is index count.
};

//LOCAL FUNCTION DECLARATIONS
/* Reads objfile in .obj format and writes the vertices and indices back.
 * returns the count of vertices and indices in the ivec2, in that order. */
struct meshld_ret readobj(FILE *objfile);

//Create a new entity, optionally with a particular ID.
int newentity(char *name, int parentid, int id)
{
	//If a parent is wanted, get a pointer to that entity.
	struct s_entity *parentptr = NULL;
	if (parentid != 0) {
		for (int i = 0; i < entityc; i++) {
			if (entity[i].id == parentid) {
				parentptr = &entity[i];
				break;
			}
		}
		if (parentptr == NULL) {
			printf("ERROR: Entity %d does not exist\n", parentid);
			return -1;
		}
	}

	//Reallocate space as nessecary.
	if (entityc % 16 == 0) {
		entity = realloc(entity, (entityc + 16) * sizeof(struct s_entity));
	}

	//If an id to use is specified, check its availability.
	int newid = idcounter;
	if (id != 0) {
		newid = id;
		for (int i = 0; i < entityc; i++) {
			if (entity[entityc].id == id) {
				printf("ERROR: Entity with id %d already "
					"exists, using the default id.\n", id);
				newid = idcounter;
				break;
			}
		}
	} else
		idcounter++;

	//Set up the new entity to be written.
	entity[entityc] = (struct s_entity){
		newid, //id
		NULL, //vert
		0, //vertc
		NULL, //index
		0, //polyc
		0, //vao
		0, //vbo
		0, //ebo,
		0, //texture
		(vec3){{0.0f, 0.0f, 0.0f}}, //pos
		(vec3){{0.0f, 0.0f, 0.0f}}, //rot
		(vec3){{1.0f, 1.0f, 1.0f}}, //scl
		1.0f, //alpha
		0, //membs
		NULL, //memb
		parentptr, //parent

	};
	//set up GL for this object.
	glGenVertexArrays(1, &entity[entityc].vao);
	glGenBuffers(1, &entity[entityc].vbo);
	glGenBuffers(1, &entity[entityc].ebo);

	glBindVertexArray(entity[entityc].vao);
	glBindBuffer(GL_ARRAY_BUFFER, entity[entityc].vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity[entityc].ebo);
	glBindVertexArray(0);

	entityc++;

	//Return the id of the entity created.
	return entity[entityc - 1].id;
}

//Remove an entity from the array and optionally remove all its members
//recursively.
int rmentity(int id, int recur)
{
	//Find eid in the entity array
	int index = -1;
	for (int i = 0; i < entityc; i++) {
		if (entity[i].id == id) {
			index = i;
		}
	}
	if (index == -1)
		printf("ERROR: Can't remove entity %d, doesn't exist.", id);

	//free the GL buffers, ready to be reallocated.
	glDeleteVertexArrays(1, &entity[index].vao);
	glDeleteBuffers(1, &entity[index].vbo);
	glDeleteBuffers(1, &entity[index].ebo);

	entityc--;

	//If you want to recursively delete the members of this object too...
	if (recur) {
		for (int i = 0; i < entity[index].membs; i++) {
			rmentity(entity[index].memb[i]->id, 1);
		}
	} else { //Otherwise just make them members of the world.
		for (int i = 0; i < entity[index].membs; i++) {
			entity[index].memb[i]->parent = NULL;
			/* Important!: This will put the member objects in a
			 * completely different place to where they were when
			 * they were when they were members, because rotations,
			 * scalings, and translations of member objects are
			 * drawn relative to their parents. It may be wise to
			 *change this eventually. */
		}
	}

	//Remove reference to this object in the parent's struct
	struct s_entity *parent = entity[index].parent;
	if (parent != NULL) {
		int pmembs = parent->membs;
		for (int i = 0; i < parent->membs; i++) {
			if (parent->memb[i]->id == id) {
				parent->memb[i] = parent->memb[pmembs - 1];
				parent->membs--;
			}
		}
	}

	//Copy the last entity into the spot where this one was...
	entity[index] = entity[entityc];
	//...and fix the references to it in other objects
	for (int i = 0; i < entity[index].membs; i++) {
		struct s_entity *member = entity[index].memb[i];
		member->parent = &entity[index];
	}
	parent = entity[index].parent;
	if (parent != NULL) {
		int pmembs = parent->membs;
		for (int i = 0; i < pmembs; i++) {
			if (parent->memb[i]->id == entity[index].id) {
				parent->memb[i] = parent->memb[pmembs - 1];
				parent->membs--;
			}
		}
	}

	//return the id of the moved entity, in case you need it.
	return entity[index].id;
}

//Read from a .obj file, put its vertex data into entity at id, and update GL.
int loadmod(char *filepath, int id)
{
	//Open the file
	char meshpath[256];
	sprintf(meshpath, "%s/mesh.obj", filepath);
	FILE *meshfile = fopen(meshpath, "r");
	if (meshfile == NULL) {
		printf("ERROR: Opening model at %s\n", meshpath);
		perror("");
		return -1;
	}
	char texpath[256];
	sprintf(texpath, "%s/tex.png", filepath);

	//Set up pointer for storing vertex data from file
	int posvertc = 1; //This is the number of ((vertices PLUS 1)).
	vec3 *posvert = NULL; //			       ^  ^ because .obj
	//						starts from 1 not 0

	//Pointer for texture vertices
	int texvertc = 1; //This is the number of ((vertices PLUS 1)).
	vec2 *texvert = NULL;

	/* Pointer to pairs of position vertices, then texture vertices, in
	 * that order. */
	int vertc = 0; //This is the number of ((pairs PLUS 1) TIMES 5).
	vertex *vert = NULL;

	int indexc = 0;
	int *index = NULL;


	struct meshld_ret newmesh = readobj(meshfile);

	vert = newmesh.vert;
	index = newmesh.index;
	vertc = newmesh.count.e[0];
	indexc = newmesh.count.e[1];


	//Find the entity to load
	int entindex;
	for (int i = 0; i < entityc; i++) {
		if (entity[i].id == id) {
			entindex = i;
		}
	}

	if (entity[entindex].vert != NULL)
		free(entity[entindex].vert);
	if (entity[entindex].index != NULL)
		free(entity[entindex].index);


	entity[entindex].vert = (float *)vert;
	entity[entindex].vertc = vertc;
	entity[entindex].index = (int *)index;
	entity[entindex].polyc = indexc / 3;

	updatemesh(&entity[entindex]);

	free(posvert);
	free(texvert);
	free(vert);
	free(index);

	fclose(meshfile);

	settex(texpath, id);

	return 0;
}

//Look at the buffer handles in ent and update them in GL
int updatemesh(struct s_entity *ent)
{
	glBindVertexArray(ent->vao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 *
		ent->vertc, ent->vert, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) *
		ent->polyc * 3, ent->index,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return 0;
}

int settex(char *filepath, int id)
{
	int entindex = -1;
	for (int i = 0; i < entityc; i++) {
		if (entity[i].id == id) {
			entindex = i;
			break;
		}
	}
	if (entindex == -1) {
		printf("ERROR: Entity %d doesn't exist, can't change texture\n", id);
		return -1;
	}
	//create a texture object
	glGenTextures(1, &entity[entindex].tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, entity[entindex].tex);
	//Set its parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Read the file
	int tex_width, tex_height, tex_chans;
	stbi_set_flip_vertically_on_load(1);
	//fileread function
	unsigned char *data = stbi_load(filepath, &tex_width, &tex_height,
		&tex_chans, 0);
	if (data == NULL) {
		printf("image data is NULL\n");
		return -2;
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

	return entity[entindex].tex;
}



//LOCAL FUNCTION DEFINITIONS
/* Reads objfile in .obj format and writes the vertices and indices back.
 * returns the count of vertices and indices in the ivec2, in that order. */
struct meshld_ret readobj(FILE *objfile)
{
	//Set up pointer for storing vertex data from file
	int posvertc = 1; //This is the number of ((vertices PLUS 1)).
	vec3 *posvert = NULL; //			       ^  ^ because .obj
	//						starts from 1 not 0

	//Pointer for texture vertices
	int texvertc = 1; //This is the number of ((vertices PLUS 1)).
	vec2 *texvert = NULL;

	vertex *vert = NULL;
	int *index = NULL;

	int vertc = 0;
	int indexc = 0;

	int a = 0; //for fixng errors with goto.
	//For every line in the file...
	char line[81];
	char *linecpy = malloc(81 * sizeof(char));
	while (fgets(line, 81, objfile)) {

		//Make a copy of that string because strtok will modify it.
		strcpy(linecpy, line);

		//get the first text segment from the line
		char *header = strtok(linecpy, " \n");
		//and jump to different places based on what the line is.
		if (!strcmp(header, "v"))
			goto header_v; //position vertex
		else if (!strcmp(header, "vt"))
			goto header_vt; //texture vertex
		else if (!strcmp(header, "f"))
			goto header_f; //face
		else //anything else.
			goto loop_end;


		header_v:
			//make a new spot for the position vertices array.
			posvert = reallocarray(posvert, posvertc + 1,
				sizeof(vec3));

			//Get the new position vertex
			vec3 newposvert;
			sscanf(line, "v %f %f %f", &newposvert.e[0],
				&newposvert.e[1], &newposvert.e[2]);

			//Put the new vertex into the array.
			posvert[posvertc] = newposvert;

			//Increment the vertex counter.
			posvertc += 1;
			goto loop_end;

		header_vt:
			//Make more space in the texture vertices array.
			texvert = reallocarray(texvert, texvertc + 1,
				sizeof(vec2));

			//Read the new texture vertex.
			vec2 newtexvert;
			sscanf(line, "vt %f %f", &newtexvert.e[0],
				&newtexvert.e[1]);

			//Add the new vertex to the array.
			texvert[texvertc] = newtexvert;

			//Increment the vertex counter.
			texvertc += 1;
			goto loop_end;

		header_f:
			a=a;
			//Make vectors for indices to the position vertices
			//array and the texture vertices array.
			ivec3 posindex;
			ivec3 texindex;
			//Read the indices to those arrays
			sscanf(line, "f %d/%d %d/%d %d/%d",
				&posindex.e[0], &texindex.e[0],
				&posindex.e[1], &texindex.e[1],
				&posindex.e[2], &texindex.e[2]);

			//For each of the corners just read...
			for (int j = 0; j < 3; j++) {
				//make a new vertex
				vertex newvertex;
				//add the components to the vertex
				newvertex.pos = posvert[posindex.e[j]];
				newvertex.col = (vec3){{0.0f, 0.0f, 0.0f}};
				newvertex.tex = texvert[texindex.e[j]];

				//do this as a check after the loop
				int oldindexc = indexc;
				//for every vertex in the big vertices array...
				for (int i = 0; i < vertc; i++) {
					/* Now pause for a moment and read the
					 * condition below.*/
					if (vert[i].pos.e[0] == //if this
						newvertex.pos.e[0] &&//vertex
						vert[i].pos.e[1] ==//matches
						newvertex.pos.e[1] &&//one in
						vert[i].pos.e[2] ==//the
						newvertex.pos.e[2] &&//array...
						vert[i].col.e[0] ==
						newvertex.col.e[0] &&
						vert[i].col.e[1] ==
						newvertex.col.e[1] &&
						vert[i].col.e[2] ==
						newvertex.col.e[2] &&
						vert[i].tex.e[0] ==
						newvertex.tex.e[0] &&
						vert[i].tex.e[1] ==
						newvertex.tex.e[1]) {
						//add more space
						index = reallocarray(index,
							indexc + 1,
							sizeof(int));
							index[indexc] = i;
							indexc += 1;
						}
	/* You might be thinking, "why not just compare the two vertices
	 * directly?", and I say that that is an elegant and rational solution
	 * to the problem of *AHEM* comparing vertices. The reason that can't
	 * be done is that the compiler shits itslef when I try:

	error: invalid operands to binary == (have ‘vertex’ {aka ‘struct
	s_vertex’} and ‘vertex’ {aka ‘struct s_vertex’})
	   239 |      if (newvertex == vert[i]) {
	       |                    ^~ ~~~~~~~
	       |                           |
	       |                           vertex {aka struct s_vertex}

	 * It can't tell the difference between two of the same thing, because.
	 * A similar error appaears when I use the s_vertex type and bypass the
	 * typedef, and also when I compare the 3 member vecs, so each of the
	 * eight total float elements of both vertices must be compared
	 * individually. */
				}
				if (indexc == oldindexc) {
					vert = reallocarray(vert, vertc + 1, sizeof(vertex));
					vert[vertc] = newvertex;
					index = realloc(index, (indexc + 1) * sizeof(int));
					index[indexc] = vertc;
					vertc += 1;
					indexc += 1;

				}
			}

			goto loop_end;
		loop_end:
			a = a;


	}
	return (struct meshld_ret){vert, index, {{vertc, indexc}}};
}
