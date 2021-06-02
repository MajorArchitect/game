#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <glad/glad.h>

#include "shader.h"


unsigned int mkshader(char *vshader_path, char *fshader_path) {
	int id;
	int success;
	char infolog[512] = {0};

	//Open files
	int vshader_fd = open(vshader_path, 0);
	if (vshader_fd == -1) {
		printf("ERROR: %s does not exist, can't read vertex shader.\n",
			vshader_path);
		return 0;
	}
	int fshader_fd = open(fshader_path, 0);
	if (fshader_fd == -1) {
		printf("ERROR: %s does not exist, can't read fragment "
			"shader.\n", vshader_path);
		return 0;
	}

	//The following 2 blocks were a pain to stop compiler warnings.
	//Read files
	const char *vshader_src = calloc(2048, sizeof(char)); //The file could be big!
	if (read(vshader_fd, (char *)vshader_src, 2047) == -1) {
		printf("ERROR: %s does exist, but couldn't read\n",
			vshader_path);
		return 0;
	}
	const char *fshader_src = calloc(2048, sizeof(char));
	if (read(fshader_fd, (char *)fshader_src, 2047) == -1) {
		printf("ERROR: %s does exist, but couldn't read\n",
			fshader_path);
		return 0;
	}

	//Compile shaders
	unsigned int vshader, fshader;
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vshader_src, NULL);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vshader, 512, NULL, infolog);
		printf("ERROR: GL: VSHADER: %s\n", infolog);
		return 0;

		glDeleteShader(vshader);
	}
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fshader_src, NULL);
	glCompileShader(fshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vshader, 512, NULL, infolog);
		printf("ERROR: GL: FSHADER: %s", infolog);

		glDeleteShader(vshader);
		glDeleteShader(fshader);
		return 0;
	}

	id = glCreateProgram();
	glAttachShader(id, vshader);
	glAttachShader(id, fshader);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infolog);
		printf("ERROR: GL: PROGRAM: %s\n", infolog);
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		glDeleteProgram(id);
		return 0;
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return id;
}
