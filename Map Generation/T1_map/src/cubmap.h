#pragma once
#ifndef __CUBMAP_H__
#define __CUBMAP_H__


struct cube_t
{
	vec3	center = vec3(0);
	float	length = 5300.0f;		// radius
	vec4	color;					// RGBA color in [0,1]
	mat4	model_matrix;			// modeling transformation

	void	update();
};

inline cube_t create_cube()
{
	cube_t cube;

	cube = { vec3(0,0,0), 500.0f, vec4(1.0f,0.5f,0.5f,1.0f) };

	return cube;
}

inline cube_t create_skybox()
{
	cube_t skybox;

	skybox = { vec3(0,0,0), 1.0f, vec4(1.0f,0.5f,0.5f,1.0f) };

	return skybox;
}


inline void cube_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		length, 0, 0, 0,
		0, length, 0, 0,
		0, 0, length, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * scale_matrix;
}
#endif
