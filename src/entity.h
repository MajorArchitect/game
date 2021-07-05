#ifndef OBJECT_H
#define OBJECT_H

#include "matvec.h"

//This holds data for entities.
struct s_entity {
	int id; //Unique object id, independent of array position.

	//Byte for bitmasking the entity's modes.
	char mode;
	//Bit 0: ENT_DOESPHYSICS - controls whether entity is affected by
		//dophysics.

	float *vert; //GL vertex buffer data
	int vertc;
	unsigned int *index; //Vertex index data
	int polyc; //Number of triangles. also number of (indices / 3)

	unsigned int vao; //GL Vertex Array Object handle
	unsigned int vbo; //GL Vertex Buffer Object handle
	unsigned int ebo; //GL Element Buffer Object handle
	unsigned int tex;

	vec3 pos; //Position relative to parent
	vec3 rot; //Rotation relative to parent
	vec3 scl; //Scale relative to parent

	vec3 vel; //Velocity vector
	vec3 acc; //Acceleration vector

	float alpha; //How transparent the object is (1 is no fade)

	int membs; //Number of members in this entity
	struct s_entity **memb; //Pointer to array of pointers to members
	struct s_entity *parent; //Pointer to single parent,
		//NULL if tied to world
};

//Create a new entity, optionally with a particular ID.
int newentity(char *name, int parentid, int id);

//Remove an entity from the array and optionally remove all its members
//recursively.
int rmentity(int id, int recur);

int getentindex(int id);

//Read from a .obj file, put its vertex data into entity at id, and update GL.
int loadmod(char *filepath, int id);

//Update the entity's GL buffers with whatever is in the mesh pointer.
int updatemesh(struct s_entity *ent);

//Read a .obj file and load that mesh into an entity.
int loadmesh(char *filepath, int id);

//Set the texture of the object id to an image in some file at filepath.
int loadtex(char *filepath, int id);

#endif //OBJECT_H
