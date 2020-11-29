#pragma once
#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__
#include "cgmath.h"

struct planet_t
{
	vec3	center = vec3(0);			// 3D position for translation
	float	radius = 1.0f;				// radius
	float	theta = 0.0f;				// rotation angle
	float	rotation_speed = 0.0f;		// rotation speed 
	float	revolution_speed = 0.0f;	// revolution speed
	vec4	color;						// RGBA color in [0,1]
	mat4	model_matrix;				// modeling transformation

	// public functions
	void	update(float t);
};

struct moon_t
{
	vec3		center = vec3(0);			// 3D position for translation
	float		radius = 1.0f;				// radius
	float		theta = 0.0f;				// rotation angle
	float		rotation_speed = 0.0f;		// rotation speed
	float		revolution_speed = 0.0f;	// revolution speed
	planet_t	planet;
	vec4		color;						// RGBA color in [0,1]
	mat4		model_matrix;				// modeling transformation

	// public functions
	void	update(float t);
};

struct ring_t
{
	vec3	center = vec3(0);		// 3D position for translation
	float	radius = 1.0f;			// radius
	float	theta = 0.0f;				// rotation angle
	float	rotation_speed = 0.0f;		// rotation speed
	float	revolution_speed = 0.0f;	// revolution speed
	vec4	color;					// RGBA color in [0,1]
	mat4	model_matrix;			// modeling transformation

	// public functions
	void	update(float t);
};

inline std::vector<planet_t> create_sun()
{
	std::vector<planet_t> circles;
	planet_t c;

	/* Sun */
	c = { vec3(0, 0, 0), 50.0f, 0.0f, 0.5f, 0.0f, vec4(1.0f,0.5f,0.5f,1.0f) };
	circles.emplace_back(c);

	return circles;
}

inline std::vector<planet_t> create_circles()
{
	std::vector<planet_t> circles;
	planet_t c;

	/* Mercury */
	c = { vec3(60.0f, 60.0f, 0), 5.0f, 0.0f, 0.3f, 4.0f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	/* Venus */
	c = { vec3(75.0f, -75.0f, 0), 7.5f, 0.0f, 0.2f, 3.0f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	/* Earth */
	c = { vec3(100.0f, 84.4f, 0), 8.0f, 0.0f, 1.0f, 2.0f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	
	// Mars
	c = { vec3(-130.0f, 123.0f, 0), 5.0f, 0.0f, 0.9f, 1.2f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	
	// Jupiter
	c = { vec3(190.2f, -183.5f, 0), 18.2f, 0.0f, 1.6f, 1.0f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	
	// Saturn 
	c = { vec3(-300.0f, -230.9f, 0), 9.4f, 0.0f, 2.0f, 0.8f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	
	// Uranus 
	c = { vec3(-380.0f, 283.0f, 0), 6.0f, 0.0f, 1.3f, 0.6f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// Neptune 
	c = { vec3(455.2f, -350.5f, 0), 4.9f, 0.0f, 1.4f, 0.4f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	
	return circles;
}

inline std::vector<moon_t> create_moons(std::vector<planet_t> planet)
{
	std::vector<moon_t> circles;
	moon_t c;

	// For EARTH
	c = { vec3(6.0f, 4.5f, 0), 1.5f, 0.0f, 0.5f, 1.0f, planet[2], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// For MARS
	c = { vec3(5.0f, 4.5f, 0), 1.4f, 0.0f, 0.5f, 1.0f, planet[3], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(6.0f, 5.8f, 0), 2.0f, 0.0f, 0.8f, 2.0f, planet[3], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// For JUPITER
	c = { vec3(25.7f, 20.7f, 0), 0.7f, 0.0f, 1.0f, 0.5f, planet[4], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(23.2f, 22.4f, 0), 0.9f, 0.0f, 0.8f, 0.6f, planet[4], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(15.1f, 16.4f, 0), 1.6f, 0.0f, 0.5f, 0.7f, planet[4], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(25.2f, 23.1f, 0), 1.2f, 0.0f, 0.3f, 0.8f, planet[4], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// For URANUS
	c = { vec3(10.7f, 18.7f, 0), 0.7f, 0.0f, 1.0f, 0.5f, planet[6], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(13.2f, 20.4f, 0), 0.9f, 0.0f, 0.8f, 0.6f, planet[6], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(15.1f, 19.4f, 0), 1.6f, 0.0f, 0.5f, 0.7f, planet[6], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(18.2f, 20.1f, 0), 1.2f, 0.0f, 0.3f, 0.8f, planet[6], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(20.2f, 20.1f, 0), 1.5f, 0.0f, 0.3f, 0.8f, planet[6], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// For NEPTUNE
	c = { vec3(15.7f, 10.7f, 0), 0.7f, 0.0f, 1.0f, 0.5f, planet[7], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	c = { vec3(23.2f, 22.4f, 0), 0.9f, 0.0f, 0.8f, 0.6f, planet[7], vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);
	return circles;
}

inline std::vector<ring_t> create_ring()
{
	std::vector<ring_t> circles;
	ring_t c;

	// For Saturn
	c = { vec3(-300.0f, -230.9f, 0), 25.4f, 0.0f, 2.0f, 0.8f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	// For Uranus
	c = { vec3(455.2f, -350.5f, 0), 15.0f, 0.0f, 1.4f, 0.4f, vec4(0.5f,1.0f,1.0f,1.0f) };
	circles.emplace_back(c);

	return circles;
}


inline void planet_t::update(float t)
{
	theta = t;
	float rotation_c = cos(rotation_speed * theta), rotation_s = sin(rotation_speed * theta);
	float revolution_c = cos(revolution_speed * theta), revolution_s = sin(revolution_speed * theta);


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
		rotation_c, -rotation_s, 0, 0,
		rotation_s, rotation_c,  0, 0,
		0,			0,			 1, 0,
		0,			0,			 0, 1
	};

	mat4 revolution_matrix =
	{
		revolution_c, -revolution_s, 0, 0,
		revolution_s, revolution_c,  0, 0,
		0,			0,				 1, 0,
		0,			0,				 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = rotation_matrix * translate_matrix * scale_matrix * revolution_matrix;
}

inline void moon_t::update(float t)
{
	theta = t;
	float rotation_c = cos(rotation_speed * theta), rotation_s = sin(rotation_speed * theta);
	float revolution_c = cos(revolution_speed * theta), revolution_s = sin(revolution_speed * theta);


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
		rotation_c, -rotation_s, 0, 0,
		rotation_s, rotation_c,  0, 0,
		0,			0,			 1, 0,
		0,			0,			 0, 1
	};

	mat4 revolution_matrix =
	{
		revolution_c, -revolution_s, 0, 0,
		revolution_s, revolution_c,  0, 0,
		0,			0,				 1, 0,
		0,			0,				 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	planet.update(t);

	mat4 c =
	{
		1, 0, 0, planet.center.x,
		0, 1, 0, planet.center.y,
		0, 0, 1, planet.center.z,
		0, 0, 0, 1
	};

	mat4 c_r =
	{
		cos(planet.rotation_speed * theta), -sin(planet.rotation_speed * theta), 0, 0,
		sin(planet.rotation_speed * theta), cos(planet.rotation_speed * theta),  0, 0,
		0,			0,				 1, 0,
		0,			0,				 0, 1
	};

	model_matrix = c_r * c * revolution_matrix * scale_matrix * rotation_matrix * translate_matrix;
}

inline void ring_t::update(float t)
{
	theta = t;
	float rotation_c = cos(rotation_speed * theta), rotation_s = sin(rotation_speed * theta);
	float revolution_c = cos(revolution_speed * theta), revolution_s = sin(revolution_speed * theta);


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
		rotation_c, -rotation_s, 0, 0,
		rotation_s, rotation_c,  0, 0,
		0,			0,			 1, 0,
		0,			0,			 0, 1
	};

	mat4 revolution_matrix =
	{
		revolution_c, -revolution_s, 0, 0,
		revolution_s, revolution_c,  0, 0,
		0,			0,				 1, 0,
		0,			0,				 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = rotation_matrix * translate_matrix * scale_matrix * revolution_matrix;

}

#endif
#pragma once

