#include "physics.h"

#include "matvec.h"
#include "entity.h"
#include "globals.h"

int runphysics(float dtime)
{
	for (int i = 0; i < entity_c; i++) {
		//gravity, tempoary
		if (entity[i].id != 1 ) {
			if (entity[i].pos.e[2] > 0.0f) {
				entity[i].acc.e[2] = -9.8f;
			} else {
				entity[i].acc.e[2] = 0.0f;
				entity[i].vel.e[2] = 0.0f;
			}
		}

		entity[i].vel.e[0] += entity[i].acc.e[0] * dtime;
		entity[i].vel.e[1] += entity[i].acc.e[1] * dtime;
		entity[i].vel.e[2] += entity[i].acc.e[2] * dtime;

		entity[i].pos.e[0] += entity[i].vel.e[0] * dtime;
		entity[i].pos.e[1] += entity[i].vel.e[1] * dtime;
		entity[i].pos.e[2] += entity[i].vel.e[2] * dtime;
	}

	return 0;
}
