#ifndef MATRIX_H
#define MATRIX_H


/* ATTENTION: please free these matrices and vectors when you are done with
 * them. The functions that work with them malloc their contents so they WILL
 * REMAIN in memory no matter where you use them.
 * For example don't do this:
 * 	mat4 mat = mat_ident(a);
 * 	mat = mat_trans(b, c);
 * The original */

/* Here, I cast off mathematical convention for the sake of logic. If I want
 * to, in real world terms, rotate an object, then scale it up, then translate,
 * it into the scene, then I will specify those functions in order:
 * 	mat4 m = mat_ident(1.0f);
 * 	m = mat_rot(m, r);
 * 	m = mat_scale(m, s);
 * 	m = mat_trans(m, t);
 * So gamedevs don't have to deal with this mathmetician's logic. The way that
 * I do this can be seen easily in mat_trans. After creating the translation
 * matrix from the vector, t goes on the left side of the multiplication with
 * m.*/

struct s_mat4 {
	float e[4][4];
};
typedef struct s_mat4 mat4;


struct s_vec2 {
	float e[2];
};
typedef struct s_vec2 vec2;

struct s_vec3 {
	float e[3];
};
typedef struct s_vec3 vec3;

struct s_vec4 {
	float e[4];
};
typedef struct s_vec4 vec4;


struct s_ivec2 {
	int e[2];
};
typedef struct s_ivec2 ivec2;

struct s_ivec3 {
	int e[3];
};
typedef struct s_ivec3 ivec3;

struct s_vertex {
	vec3 pos;
	vec3 col;
	vec2 tex;
};
typedef struct s_vertex vertex;


vec3 vec_add(vec3 a, vec3 b); vec2 vec2_add(vec2 a, vec2 b);
vec3 vec_sub(vec3 a, vec3 b); vec2 vec2_sub(vec2 a, vec2 b);
vec3 vec_scale(vec3 v, float s); vec2 vec2_scale(vec2 v, float s);
vec3 vec_norm(vec3 v); vec2 vec2_norm(vec2 v);
float vec_dot(vec3 a, vec3 b); float vec2_dot(vec2 a, vec2 b);
vec3 vec_cross(vec3 a, vec3 b);

mat4 mat_ident(float s);
mat4 mat_trans(mat4 m, vec3 t);
mat4 mat_scale(mat4 m, vec3 s);
mat4 mat_rot(mat4 m, float r, vec3 a);

mat4 mat_proj(mat4 m, float ar, float fov, float znear, float zfar);
mat4 mat_lookat(mat4 m, vec3 pos, vec3 target, vec3 wup);

vec4 matvecprod(mat4 m, vec4 v);
mat4 matmatprod(mat4 l, mat4 r);

#endif //MATRIX_H
