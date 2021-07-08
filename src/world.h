#ifndef WORLD_H
#define WORLD_H

#include "matvec.h"

void makeworld();

void drawworld(mat4 view, mat4 proj);

//Returns the height of the triangle at point coord on the map.
float terrainheight(vec2 coords);

void freeworld();

#endif // WORLD_H
