#pragma once
#ifndef __DESCRIPTION_H__
#define __DESCRIPTION_H__
#include "cgmath.h"


struct desc_t
{
	vec2	center = vec2(0);
	float	length = 100.0f;		// radius
	vec3	color;					// RGBA color in [0,1]
	mat4	model_matrix;			// modeling transformation

	void	update();
};

inline std::vector<desc_t> create_desc()
{
	std::vector<desc_t> descs;
	desc_t d;

	d = { vec2(0.0f,0.0f), 100.0f, vec3(1.0f,0.5f,0.5f) };
	descs.emplace_back(d);
	//d = { vec2(0,-14900.0f), 100.0f, vec3(1.0f,0.5f,0.5f) };
	//descs.emplace_back(d);
	//d = { vec2(150.0f, -14900.0f), 100.0f, vec3(1.0f,0.5f,0.5f) };
	//descs.emplace_back(d);
	return descs;
}

inline void desc_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		length, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, length, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * scale_matrix;
}
#endif
