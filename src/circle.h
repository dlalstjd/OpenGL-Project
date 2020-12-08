#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__
#include "cgmath.h"
#include "hexgrid.h"	
#include "npc.h"
#define RANGE float(sqrt((3))) / float(2)


struct test_obj
{
	vec3	center = vec3(0);			// 3D position for translation
	float	radius = 1.0f;				// radius
	float	theta = 0.0f;				// rotation angle
	float	gravity = 0.0f;					// rotation speed 
	vec4	color;						// RGBA color in [0,1]
	mat4	model_matrix;				// modeling transformation

	float	HP = 500.0f;
	// public functions
	void	update(float t, bool started);
	bool	top_collide(std::vector<hexcell_t> hexgrid);
	bool	middle_collide(std::vector<hexcell_t> hexgrid);
	bool	bottom_collide(std::vector<hexcell_t> hexgrid);
	void	if_collide_npc(NPC npc);

};

inline test_obj create_test_obj()
{
	test_obj c;
	c = { vec3(0.0f, 0.0f, 40.0f), 0.07f, 0.0f, 2.0f, vec4(1.0f,0.5f,0.5f,1.0f) };
	return c;
}

inline void test_obj::update(float t, bool started)
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

	mat4 standup_matrix =
	{
	   1, 0, 0, 0,
	   0, cosf(PI / 2), -1 * sinf(PI / 2), 0,
	   0, sinf(PI / 2), cosf(PI / 2), 0,
	   0, 0, 0, 1
	};

	mat4 showback_matrix =
	{
	   cosf(PI), -1 * sinf(PI), 0, 0,
	   sinf(PI), cosf(PI), 0, 0,
	   0, 0, 1, 0,
	   0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	if (started)
		center.z = center.z - 0.01f * gravity;

	mat4 centering_matrix = {
		1, 0, 0, 0.1f,
		0,1,0, 0.1f,
		0,0,1,-0.5f,
		0,0,0,1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
	model_matrix = centering_matrix * translate_matrix * rotation_matrix * showback_matrix * standup_matrix * scale_matrix;
}

float distance_(vec2 object, hexcell_t cell, uint k) {
	vec2 tmp = vec2(object.x - cell.center.x, object.y - cell.center.y);
	//vec2 norm = vec2(cell.norm[k].x - cell.center.x, cell.norm[k].y - cell.center.y);
	vec2 result = vec2(tmp.x * cell.norm[k].x, tmp.y * cell.norm[k].y);
	if (tmp.length() > 1.0f) return 1000.0f;
	return result.length();
}

bool test_obj::top_collide(std::vector<hexcell_t> hexgrid) {
	float point;
	if (center.z > 21.2f || center.z < 21.1f) {
		gravity += 0.0005f;
		return false;
	}
	for (auto& c : hexgrid) {
		for (uint k = 0; k < 6; k++) {
			point = distance_(vec2(center.x, center.y), c, k);
			if (point <= RANGE) {
				gravity = 0.0f;
				//center.z = 21.2f;
				return true;
			}
		}
	}
	gravity += 0.0005f;
	return false;
}

bool test_obj::middle_collide(std::vector<hexcell_t> hexgrid) {
	float point;
	if (center.z > 1.2f || center.z < 1.1f) {
		gravity += 0.0005f;
		return false;
	}
	for (auto& c : hexgrid) {
		for (uint k = 0; k < 6; k++) {
			point = distance_(vec2(center.x, center.y), c, k);
			if (point <= RANGE) {
				gravity = 0.0f;
				//center.z = 1.2f;
				return true;
			}
		}
	}
	gravity += 0.0005f;
	return false;
}

bool test_obj::bottom_collide(std::vector<hexcell_t> hexgrid) {
	float point;
	if (center.z > -18.8f) {
		gravity += 0.0005f;
		return false;
	}
	for (auto& c : hexgrid) {
		for (uint k = 0; k < 6; k++) {
			point = distance_(vec2(center.x, center.y), c, k);

			if (point <= RANGE) {
				gravity = 0.0f;
				//center.z = -18.8f;
				return true;
			}
		}
	}
	gravity += 0.0005f;
	return false;
}

void test_obj::if_collide_npc(NPC npc) {
	vec3 result = vec3(center.x - npc.center.x, center.y - npc.center.y, center.z - npc.center.z);
	if (result.length() < 2.0f) {
		HP -= 0.05f;
		//printf("hp: %lf\n", HP);
	}
}

#endif
#pragma once