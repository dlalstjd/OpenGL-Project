#pragma once
#ifndef __COLLISIONDECTION_H__
#define __COLLISIONDECTION_H__

#include "circle.h"
#include "hexgrid.h"
#include "cgmath.h"
#include "cgut.h"
#include <float.h>

#define RANGE float(sqrt((3))) / float(2)


float distance(vec2 object, hexcell_t cell, uint k) {
	vec2 tmp = vec2(object.x - cell.center.x, object.y - cell.center.y);
	//vec2 norm = vec2(cell.norm[k].x - cell.center.x, cell.norm[k].y - cell.center.y);
	vec2 result = vec2(tmp.x * cell.norm[k].x, tmp.y * cell.norm[k].y);
	if (tmp.length() > 1.0f) return 1000.0f;
	return result.length();
} 

std::vector<hexcell_t> dispear(std::vector<hexcell_t> hexgrid, uint index) {
	hexgrid.erase(hexgrid.begin() + index);
	return hexgrid;
}

std::vector<hexcell_t> after_collide(test_obj test, std::vector<hexcell_t> hexgrid) {
	float point = 0.0f;
	//std::vector<hexcell_t>::iterator index = hexgrid.begin();
	uint index = 0;
	bool flag = false;

	for (auto& c : hexgrid) {
		for (uint k = 0; k < 6; k++) {
			point = distance(vec2(test.center.x, test.center.y), c, k);
			//printf("%f\n", point);
			if (point < RANGE) {
				c.timer();
				//printf("%f %f %f\n", c.center.x, c.center.y, c.time);
				flag = true;
				break;
			}
		}
		if (flag)break;
		index++;
	}
	return hexgrid;
}

//std::vector<hexcell_t> after_collide(test_obj test, std::vector<hexcell_t> hexgrid) {
//	float point = 0.0f;
//	//std::vector<hexcell_t>::iterator index = hexgrid.begin();
//	uint index = 0;
//	bool flag = false;
//
//	for (auto& c : hexgrid) {
//		for (uint k = 0; k < 6; k++) {
//			point = distance(vec2(test.center.x, test.center.y), c, k);
//			if (point < RANGE) {
//				flag = true;
//				break;
//			}
//		}
//		if (flag)break;
//		index++;
//	}
//
//	return dispear(hexgrid, index);
//}

#endif
