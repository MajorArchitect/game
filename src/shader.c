#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <glad/glad.h>

#include "shader.h"


unsigned int mkshader(char *vshader_path, char *fshader_path) {
	char *vshader_src = calloc(4096, sizeof(char));
	char *fshader_src = calloc(4096, sizeof(char));


	FILE *vshader_file = fopen(vshader_path, "r");
	if (vshader_file == NULL) {
		perror("ERROR: opening vertex shader");
		return 0;
	}

	FILE *fshader_file = fopen(fshader_path, "r");
	if (fshader_file == NULL) {
		perror("ERROR: opening fragment shader");
		return 0;
	}
	fread(vshader_src, 1, 4096, vshader_file);
	fread(fshader_src, 1, 4096, fshader_file);


	unsigned int vshader, fshader;
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, (const GLchar * const *)&vshader_src, NULL);
	glCompileShader(vshader); // ^  ^  ^ because of compiler warnings
	int success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		char infolog[1024];
		int loglen = 0;
		glGetShaderInfoLog(vshader, 1024, &loglen, infolog);
		printf("ERROR: Vertex shader compilation:\n%s\n", infolog);
	}

	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, (const GLchar * const *)&fshader_src, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		char infolog[1024];
		int loglen = 0;
		glGetShaderInfoLog(fshader, 1024, &loglen, infolog);
		printf("ERROR: Fragment shader compilation:\n%s\n", infolog);
	}


	unsigned int progid = glCreateProgram();
	glAttachShader(progid, vshader);
	glAttachShader(progid, fshader);
	glLinkProgram(progid);
	glGetProgramiv(progid, GL_LINK_STATUS, &success);
	if (success != GL_TRUE) {
		char infolog[1024];
		int loglen = 0;
		glGetShaderInfoLog(fshader, 1024, &loglen, infolog);
		printf("ERROR: Shader program linking:\n%s\n", infolog);
		return 0;
	}

	return progid;

}
