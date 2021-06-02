#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

/* This returns 0 on any failure and the ID for a shader on success. The reason
 * I can't return negative numbers is because program and shader IDs are
 * unsigned int, so they could end up begin a value like -1 if GL decides so.*/
unsigned int mkshader(char *vshader_path, char *fshader_path);

#endif //SHADER_H
