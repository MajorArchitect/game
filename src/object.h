#ifndef OBJECT_H
#define OBJECT_H

#include "matvec.h"

struct object {
	float *vert;
	unsigned int *indices;
	vec3 pos;
	vec3 rot;
	float scale;
	float fade;
	struct object *members;
	struct object *parent;
}

#endif //OBJECT_H
