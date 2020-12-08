#pragma once
#ifndef __KALBI_H__
#define __KALBI_H__
#include "cgmath.h"
#include "hexgrid.h"
#include "circle.h"
#include "npc.h"
#define RANGE float(sqrt((3))) / float(2)


struct kalbi
{
	vec3	center = vec3(0);			// 3D position for translation
	vec3	shoot_point = vec3(0);
	float	radius = 1.0f;				// radius
	float	theta = 0.0f;				// rotation angle
	float	gravity = 0.0f;					// rotation speed 
	float	veloticy = 0.0f;
	float	throw_angle = PI / 4.f;
	float	t_shoot = 0.0f;
	bool	used = false;
	bool	hit = false;
	vec4	color;						// RGBA color in [0,1]
	mat4	model_matrix;				// modeling transformation

	float	HP = 500.0f;
	// public functions
	void	update(float t);// , bool started);
	bool	if_collide_npc(NPC& npc, uint level);

};

inline std::vector<kalbi> create_kalbis()
{
	std::vector<kalbi> kalbis;
	kalbi k;
	for (uint i = 0; i < 3; i++) {
		k = { vec3(0), vec3(0), 0.4f, 0.0f, 45.0f, 16.0f, PI / 4.f, 0.0f, false, false, vec4(1.0f,0.5f,0.5f,1.0f) };
		kalbis.push_back(k);
	}
	return kalbis;
}

inline void kalbi::update(float t)//, bool started)
{
	//theta = t;
	//float rotation_c = cos(rotation_speed * theta), rotation_s = sin(rotation_speed * theta);
	//float revolution_c = cos(revolution_speed * theta), revolution_s = sin(revolution_speed * theta);
	float c = cos(theta), s = sin(theta);
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

bool kalbi::if_collide_npc(NPC& npc, uint level) {
	vec3 result = vec3(center.x - npc.center.x, center.y - npc.center.y, center.z - npc.center.z);
	if (result.length() < 2.0f) {
		npc.HP -= 15.0f / float(level);
		//printf("hit npc %d \n",npc.id);
		return true;
	}
	return false;
}

#endif
#pragma once