#pragma once
#ifndef __HEXGRID_H__
#define __HEXGRID_H__



struct hexcell_t
{
	vec3		center = vec3(0);
	float		radius = 1.0f;		// radius
	vec4		color;				// RGBA color in [0,1]
	mat4		model_matrix;		// modeling transformation

	std::vector<vec2>	norm;

	float	time = 0.0f;

	void	update();
	void	normVector();
	void	timer();
};

vec2 internal_division(vec2 v1, vec2 v2, float m, float n) {
	float x1 = v1.x, y1 = v1.y;
	float x2 = v2.x, y2 = v2.y;

	vec2 result = (0);
	result.x = (x1 * n + x2 * m) / (m + n);
	result.y = (y1 * n + y2 * m) / (m + n);
	return result;
}

void hexcell_t::normVector()
{
	vec2 v1 = vec2(0);
	vec2 v2 = vec2(0);
	vec2 result;

	for (uint k = 0; k < 6; k++) {
		float t = PI * 2.0f * k / float(6) + PI * 1.0f * 1 / float(6), c = cos(t), s = sin(t);
		v1 = vec2(c, s);
		t = PI * 2.0f * (k+1) / float(6) + PI * 1.0f * 1 / float(6), c = cos(t), s = sin(t);
		v2 = vec2(c, s);
		result = internal_division(v1, v2, 1, 1) ;
		norm.push_back(result);
	}	
}

inline std::vector<hexcell_t> create_description()
{
	std::vector<hexcell_t> cells;
	hexcell_t cell;

	cell = { vec3(-35.0f, -14900.0f, 20.0f), 20.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
	cells.emplace_back(cell);

	cell = { vec3(0.0f, -14900.0f, 20.0f), 20.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
	cells.emplace_back(cell);

	cell = { vec3(35.0f, -14900.0f, 20.0f), 20.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
	cells.emplace_back(cell);

	return cells;
}

inline std::vector<hexcell_t> create_hexgrid_top()
{
	float	r = 0.0f;
	vec2	cnt = vec2(0);
	std::vector<hexcell_t> cells;
	hexcell_t cell;

	cell = { vec3(0,0, 20), 1.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
	cell.normVector();
	cells.emplace_back(cell);
	r += (float)sqrt(3);
	for (int j = 0; j < 6; j++) {
		float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
		cell = { vec3(c,s, 20), 1.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
		cell.normVector();
		cells.emplace_back(cell);
	}
	for (int i = 2; i < 5; i++) {
		r += (float)sqrt(3);
		for (int j = 0; j < 6; j++) {
			float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
			float _t = PI * 2.0f * (j + 1) / float(6), _c = r * cos(_t), _s = r * sin(_t);

			for (int k = 0; k < i - 1; k++) {
				cnt = internal_division(vec2(c, s), vec2(_c, _s), (float)(i - 1 - k), (float)(k + 1));
				cell = { vec3(cnt.x, cnt.y, 20), 1.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
				cell.normVector();
				cells.emplace_back(cell);
			}
			cell = { vec3(c,s, 20), 1.0f, vec4(0.878431f,1.0f,1.0f,1.0f) };
			cell.normVector();
			cells.emplace_back(cell);
		}
	}
	return cells;
}

inline std::vector<hexcell_t> create_hexgrid_middle()
{
	float	r = 0.0f;
	vec2	cnt = vec2(0);
	std::vector<hexcell_t> cells;
	hexcell_t cell;

	cell = { vec3(0,0,0), 1.0f, vec4(0.980392f, 0.980392f, 0.823529f,1.0f) };
	cell.normVector();
	cells.emplace_back(cell);
	r += (float)sqrt(3);
	for (int j = 0; j < 6; j++) {
		float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
		cell = { vec3(c,s, 0), 1.0f, vec4(0.980392f, 0.980392f, 0.823529f,1.0f) };
		cell.normVector();
		cells.emplace_back(cell);
	}
	for (int i = 2; i < 8; i++) {
		r += (float)sqrt(3);
		for (int j = 0; j < 6; j++) {
			float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
			float _t = PI * 2.0f * (j + 1) / float(6), _c = r * cos(_t), _s = r * sin(_t);

			for (int k = 0; k < i - 1; k++) {
				cnt = internal_division(vec2(c, s), vec2(_c, _s), (float)(i - 1 - k), (float)(k + 1));
				cell = { vec3(cnt.x, cnt.y, 0), 1.0f, vec4(0.980392f, 0.980392f, 0.823529f,1.0f) };
				cell.normVector();
				cells.emplace_back(cell);
			}
			cell = { vec3(c,s, 0), 1.0f, vec4(0.980392f, 0.980392f, 0.823529f,1.0f) };
			cell.normVector();
			cells.emplace_back(cell);
		}
	}
	return cells;
}

inline std::vector<hexcell_t> create_hexgrid_bottom()
{
	float	r = 0.0f;
	vec2	cnt = vec2(0);
	std::vector<hexcell_t> cells;
	hexcell_t cell;

	cell = { vec3(0,0, -20), 1.0f, vec4(1.0f, 0.713725f, 0.756863f, 1.0f) };
	cell.normVector();
	cells.emplace_back(cell);
	r += (float)sqrt(3);
	for (int j = 0; j < 6; j++) {
		float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
		cell = { vec3(c,s, -20), 1.0f, vec4(1.0f, 0.713725f, 0.756863f, 1.0f) };
		cell.normVector();
		cells.emplace_back(cell);
	}
	for (int i = 2; i < 10; i++) {
		r += (float)sqrt(3);
		for (int j = 0; j < 6; j++) {
			float t = PI * 2.0f * j / float(6), c = r * cos(t), s = r * sin(t);
			float _t = PI * 2.0f * (j + 1) / float(6), _c = r * cos(_t), _s = r * sin(_t);

			for (int k = 0; k < i - 1; k++) {
				cnt = internal_division(vec2(c, s), vec2(_c, _s), (float)(i - 1 - k), (float)(k + 1));
				cell = { vec3(cnt.x, cnt.y, -20), 1.0f, vec4(1.0f, 0.713725f, 0.756863f, 1.0f) };
				cell.normVector();
				cells.emplace_back(cell);
			}
			cell = { vec3(c,s, -20), 1.0f, vec4(1.0f, 0.713725f, 0.756863f, 1.0f) };
			cell.normVector();
			cells.emplace_back(cell);
		}
	}
	return cells;
}

inline void hexcell_t::timer() {
	if (time == 0.0f)
		time = float(glfwGetTime());
	//else printf("not zero");
}



inline void hexcell_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
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
