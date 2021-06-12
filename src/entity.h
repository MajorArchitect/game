#ifndef OBJECT_H
#define OBJECT_H

#include "matvec.h"

struct s_entity {
	int id; //Unique object id, independent of array position
	float *vert; //GL vertex buffer data
	int vertc;
	unsigned int *index; //Vertex index data
	int polyc; //Number of triangles. also number of (indices / 3)
	unsigned int vao; //GL Vertex Array Object handle
	unsigned int vbo; //GL Vertex Buffer Object handle
	unsigned int ebo; //GL Element Buffer Object handle
	unsigned int texture;
	vec3 pos; //Position relative to parent
	vec3 rot; //Rotation relative to parent
	vec3 scale; //Scale relative to parent
	float alpha; //How transparent the object is (1 is no fade)
	int membs; //Number of members in this entity
	struct s_entity **memb; //Pointer to array of pointers to members
	struct s_entity *parent; //Pointer to single parent,
		//NULL if tied to world
};

int newentity(char *name, int parentid, int id);
int rmentity(int id, int recur);

int loadobj(char *filepath, int id);

#endif //OBJECT_H