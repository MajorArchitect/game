#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "globals.h"
#include "shader.h"
#include "matvec.h"
#include "entity.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

struct s_entity *entity = NULL;
unsigned int entityc = 0;

float ctime;
float ptime;
float dtime;

vec3 cam_pos = {{0.0f, 0.0f, 0.0f}};
vec3 cam_front = {{0.0f, 1.0f, 0.0f}};
vec3 cam_up = {{0.0f, 0.0f, 1.0f}};
float cam_pitch = 0.0f;
float cam_yaw = 3.14159 / 2;

int main()
{

	//Opengl stuff
	glfwInit();
	ptime = glfwGetTime();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow *window = glfwCreateWindow(320, 240, "Game",
		NULL, NULL);
	if (window == NULL) {
		printf("Could not create window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialise GLAD\n");
		return -1;
	}
	glViewport(0, 0, 320, 240);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowAspectRatio(window, 4, 3);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	unsigned int shaderprog = mkshader("res/vert.vs", "res/frag.fs");
	if (shaderprog == 0) {
		printf("ERROR: mkshader returned zero.\n");
		exit(100);
	}

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderprog);

	//Make textures
	unsigned int tex1, tex2;
	//Texture 1
	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	//Set its parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Read the file
	int tex_width, tex_height, tex_chans;
	stbi_set_flip_vertically_on_load(1);
	//fileread function
	unsigned char *data = stbi_load("res/brick.png", &tex_width,
		&tex_height, &tex_chans, 0);
	if (data == NULL) {
		printf("image data is NULL\n");
		return -1;
	}
	//Set its data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);



	//TEST: Another texture
	glGenTextures(1, &tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("res/face.jpg", &tex_width, &tex_height, &tex_chans, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture");
	}
	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(shaderprog, "tex_sam1"), 0);
	glUniform1i(glGetUniformLocation(shaderprog, "tex_sam2"), 1);


	vec3 rotaxis = {{1.0f, 0.0f, 0.0f}};
	mat4 ident = mat_ident(1.0f);
	mat4 model = mat_rot(ident, 0.0f, rotaxis);
	mat4 view = ident;
	mat4 proj = mat_proj(ident, 320.0f / 240.0f, 1.57, 0.1f, 1000.0f);

	unsigned int model_loc = glGetUniformLocation(shaderprog, "model");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, (float *)model.e);
	unsigned int view_loc = glGetUniformLocation(shaderprog, "view");
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, (float *)view.e);
	unsigned int proj_loc = glGetUniformLocation(shaderprog, "proj");
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, (float *)proj.e);

	int axeid = newentity("axe", 0, 0);
	loadmod("res/axe.obj", axeid);
	int cubeid = newentity("cube", 0, 0);
	loadmod("res/cube.obj", cubeid);

	entity[cubeid - 1].scale = (vec3){{0.5f, 0.5f, 0.5f}};
	entity[cubeid - 1].pos = (vec3){{3.0f, 3.0f, 3.0f}};

	//game loop
	while (!glfwWindowShouldClose(window)) {
		ctime = glfwGetTime();
		dtime = ctime - ptime;
		ptime = ctime;
		processInput(window);

		glClearColor(0.529f, 0.808f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2);
		glUseProgram(shaderprog);

		view = mat_lookat(ident, cam_pos, vec_add(cam_pos, cam_front), (vec3){{0.0f, 0.0f, 1.0f}});
		glUniformMatrix4fv(view_loc, 1, GL_TRUE, (float *)view.e);

		for (int i = 0; i < entityc; i++) {
			glBindVertexArray(entity[i].vao);
			model = mat_scale(ident, entity[i].scale);
			model = mat_rot(model, entity[i].rot.e[0],
				(vec3){{1.0f, 0.0f, 0.0f}});
			model = mat_rot(model, entity[i].rot.e[1],
				(vec3){{0.0f, 1.0f, 0.0f}});
			model = mat_rot(model, entity[i].rot.e[2],
				(vec3){{0.0f, 0.0f, 1.0f}});
			model = mat_trans(model, entity[i].pos);

			glUniformMatrix4fv(model_loc, 1, GL_TRUE,
				(float *)model.e);

			glDrawElements(GL_TRIANGLES, entity[i].polyc * 3,
				GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Nasty hack here to clear memory at end of program
	glDeleteVertexArrays(1, &entity[0].vao);
	glDeleteBuffers(1, &entity[0].vbo);
	glDeleteBuffers(1, &entity[0].ebo);
	glDeleteProgram(shaderprog);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}


float last_xpos = 160, last_ypos = 120;
int firstmouse = 1;
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if(firstmouse) {
		last_xpos = xpos;
		last_ypos = ypos;
		firstmouse = 0;
	}
	float xoffset = xpos - last_xpos;
	float yoffset = last_ypos - ypos;
	last_xpos = xpos;
	last_ypos = ypos;

	const float sens = 0.005f;
	xoffset *= sens;
	yoffset *= sens;

	cam_yaw += xoffset;
	cam_pitch += yoffset;

	if (cam_pitch > 1.5f)
		cam_pitch = 1.5f;
	if (cam_pitch < -1.5f)
		cam_pitch = -1.5f;

	vec3 dir = {{cos(cam_yaw) * cos(cam_pitch),
		-sin(cam_yaw) * cos(cam_pitch),
		sin(cam_pitch)}};
	cam_front = vec_norm(dir);


}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	const float cam_speed = 2.5f;
	const float cam_displacement = dtime * cam_speed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam_pos = vec_add(cam_pos, vec_scale(cam_front, cam_displacement));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam_pos = vec_sub(cam_pos, vec_scale(cam_front, cam_displacement));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam_pos = vec_add(cam_pos, vec_scale(
			vec_norm(vec_cross(cam_front, cam_up)), cam_displacement));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam_pos = vec_sub(cam_pos, vec_scale(
			vec_norm(vec_cross(cam_front, cam_up)), cam_displacement));
}
