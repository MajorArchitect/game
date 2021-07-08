#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matvec.h"


vec3 vec_add(vec3 a, vec3 b)
{
	return (vec3){{a.e[0] + b.e[0], a.e[1] + b.e[1], a.e[2] + b.e[2]}};
}

vec3 vec_sub(vec3 a, vec3 b)
{
	return (vec3){{a.e[0] - b.e[0], a.e[1] - b.e[1], a.e[2] - b.e[2]}};
}

vec3 vec_scale(vec3 v, float s)
{
	return (vec3){{v.e[0] * s, v.e[1] * s, v.e[2] * s}};
}

vec3 vec_norm(vec3 v)
{
	vec3 o = v;
	float x2 = o.e[0] * o.e[0], y2 = o.e[1] * o.e[1], z2 = o.e[2] * o.e[2];
	float l = sqrt(x2 + y2 + z2);
	o.e[0] /= l, o.e[1] /= l, o.e[2] /= l;
	return o;
}

vec2 vec2_norm(vec2 v)
{
	vec2 o = v;
	float x2 = o.e[0] * o.e[0], y2 = o.e[1] * o.e[1];
	float l = sqrt(x2 + y2);
	o.e[0] /= l, o.e[1] /= l;
	return o;
}

vec3 vec_cross(vec3 a, vec3 b)
{
	vec3 c;
	c.e[0] = a.e[1] * b.e[2] - a.e[2] * b.e[1];
	c.e[1] = a.e[2] * b.e[0] - a.e[0] * b.e[2];
	c.e[2] = a.e[0] * b.e[1] - a.e[1] * b.e[0];
	return c;
}

mat4 mat_ident(float s)
{
	mat4 o = {{{s, 0.0f, 0.0f, 0.0f},
		{0.0f, s, 0.0f, 0.0f},
		{0.0f, 0.0f, s, 0.0f},
		{0.0f, 0.0f, 0.0f, s}}};
	return o;
}

mat4 mat_trans(mat4 m, vec3 v)
{
	mat4 t = mat_ident(1.0f);

	for (int i = 0; i < 3; i++)
		t.e[i][3] = v.e[i];
	mat4 o;
	o = matmatprod(t, m);
	return o;
}

mat4 mat_scale(mat4 m, vec3 v)
{
	mat4 s = mat_ident(1.0f);

	for (int i = 0; i < 3; i++)
		s.e[i][i] = v.e[i];

	mat4 o = matmatprod(s, m);

	return o;
}

mat4 mat_rot(mat4 m, float rad, vec3 a)
{
	float s = sinf(rad);
	float c = cosf(rad);
	float t = 1.0f - c;
	//float ux = a.e[0];
	//float uy = a.e[1];
	//float uz = a.e[2];
	float tux2 = t * a.e[0] * a.e[0];
	float tuy2 = t * a.e[1] * a.e[1];
	float tuz2 = t * a.e[2] * a.e[2];
	float tuxuy = t * a.e[0] * a.e[1];
	float tuxuz = t * a.e[0] * a.e[2];
	float tuyuz = t * a.e[1] * a.e[2];
	float sux = s * a.e[0];
	float suy = s * a.e[1];
	float suz = s * a.e[2];

	mat4 r = {{
		{tux2 + c, tuxuy - suz, tuxuz + suy, 0.0f},
		{tuxuy + suz, tuy2 + c, tuyuz - sux, 0.0f},
		{tuxuz - suy, tuyuz + sux, tuz2 + c, 0.0f},
		{0.0f,       0.0f,       0.0f,       1.0f}}};

	mat4 o = matmatprod(r, m);
	return o;
}

mat4 mat_proj(mat4 m, float ar, float fov, float znear, float zfar)
{
	float tanscale = 1 / tanf(fov / 2);
	float planes = zfar / (zfar - znear);
	mat4 p = {{
		{tanscale / ar,  0.0f,   0.0f,     0.0f       },
		{0.0f,           0.0f,   tanscale, 0.0f       },
		{0.0f,          -planes, 0.0f,     -planes * znear},
		{0.0f,          -1.0f,   0.0f,     0.0f       }}};
	mat4 o = matmatprod(p, m);
	return o;
}

mat4 mat_lookat(mat4 m, vec3 pos, vec3 target, vec3 wup)
{
	vec3 dir = vec_norm(vec_sub(pos, target));
	vec3 right = vec_norm(vec_cross(wup, dir));
	vec3 up = vec_cross(dir, right);

	mat4 r = {{
		{right.e[0], right.e[1], right.e[2], 0.0f},
		{dir.e[0],   dir.e[1],   dir.e[2],   0.0f},
		{up.e[0],    up.e[1],    up.e[2],    0.0f},
		{0.0f,       0.0f,       0.0f,       1.0f}}};
	mat4 t = {{
		{1.0f, 0.0f, 0.0f, -pos.e[0]},
		{0.0f, 1.0f, 0.0f, -pos.e[1]},
		{0.0f, 0.0f, 1.0f, -pos.e[2]},
		{0.0f, 0.0f, 0.0f,  1.0f    }}};
	mat4 l = matmatprod(r, t);
	mat4 o = matmatprod(l, m);
	return o;
}

vec4 matvecprod(mat4 m, vec4 v)
{
	vec4 o;
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
			o.e[i] += (m.e[i][k] * v.e[k]);
		}

	}
	return o;
}

mat4 matmatprod(mat4 l, mat4 r)
{
	mat4 o = mat_ident(0.0f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				o.e[i][j] += (l.e[i][k] * r.e[k][j]);
			}
		}
	}
	return o;
}
