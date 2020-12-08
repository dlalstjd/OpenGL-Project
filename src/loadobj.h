#pragma once
#ifndef __LOADOBJ_H__
#define __LOADOBJ_H__
#include "cgmath.h"				// slee's simple math library
#include "cgut.h"				// slee's OpenGL utility

#define OBJ_FILENAME_LENGTH 500
#define MATERIAL_NAME_SIZE 255
#define OBJ_LINE_SIZE 500

char* folder_path = "../bin/model/";
bool loadMTL(char* path, std::vector<char*>& mtl_names);

struct obj_material
{
	char name[MATERIAL_NAME_SIZE];
	char texture_filename[OBJ_FILENAME_LENGTH];
	vec3 amb;
	vec3 diff;
	vec3 spec;
	vec3 trans_f;
	float trans;
	float shiny;
	float glossy;
	uint refract_index;
};

bool loadOBJ(const char* path, std::vector<vec3>& out_vertices, std::vector<vec2>& out_uvs, std::vector<vec3>& out_normals, std::vector<uint>& out_indices, std::vector<char*>& out_materials) {

	printf("Loading OBJ file %s...\n", path);
	std::vector<uint> vertexIndices, uvIndices, normalIndices;
	std::vector<vec3> temp_vertices;
	std::vector<vec2> temp_uvs;
	std::vector<vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	uint cnt = 0;
	uint v_cnt = 0;
	float vx_sum = 0.f;
	float vy_sum = 0.f;
	float vz_sum = 0.f;
	vec3 mean = vec3(0);

	while (1) {

		char lineHeader[128];
		char mtlFile[500];
		char current_mtl_name[500];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "mtllib") == 0) {
			//char mtlFile[500];
			fscanf(file, "%s\n", mtlFile);
			char mtlfile_path[500];
			strcpy(mtlfile_path, folder_path);
			strcat(mtlfile_path, mtlFile);
			printf("mtlFile: %s\n", mtlfile_path);
			bool res = loadMTL(mtlfile_path, out_materials);
			//out_materials.push_back(mtlFile);
		}

		else if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vx_sum += vertex.x;
			vy_sum += vertex.y;
			vz_sum += vertex.z;
			v_cnt++;
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}

		else if (strcmp(lineHeader, "f") == 0) {

			std::string vertex1, vertex2, vertex3;
			uint vertexIndex[4], uvIndex[4], normalIndex[4];
			//uint vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			//printf("matches %d\n", matches);
			if (matches == 9) {

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
				out_indices.push_back(cnt);
				out_indices.push_back(cnt + 1);
				out_indices.push_back(cnt + 2);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				cnt += 3;
			}
			else if (matches == 12) {

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
				vertexIndices.push_back(vertexIndex[2]);
				vertexIndices.push_back(vertexIndex[3]);
				vertexIndices.push_back(vertexIndex[0]);
				uvIndices.push_back(uvIndex[2]);
				uvIndices.push_back(uvIndex[3]);
				uvIndices.push_back(uvIndex[0]);
				normalIndices.push_back(normalIndex[2]);
				normalIndices.push_back(normalIndex[3]);
				normalIndices.push_back(normalIndex[0]);
				out_indices.push_back(cnt);
				out_indices.push_back(cnt + 1);
				out_indices.push_back(cnt + 2);
				out_indices.push_back(cnt + 3);
				out_indices.push_back(cnt + 4);
				out_indices.push_back(cnt + 5);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				out_materials.push_back(current_mtl_name);
				cnt += 6;
			}

		}
		else if (strcmp(lineHeader, "usemtl") == 0) {
			fscanf(file, "%s\n", current_mtl_name);
			//strncpy(current_mtl.name, strtok(NULL, " \t"), MATERIAL_NAME_SIZE);
			//out_materials.push_back()
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	mean = vec3(vx_sum / v_cnt, 0.f, vz_sum / v_cnt);

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		uint vertexIndex = vertexIndices[i];
		uint uvIndex = uvIndices[i];
		uint normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		vec3 vertex = temp_vertices[vertexIndex - 1] - mean;
		vec2 uv = temp_uvs[uvIndex - 1];
		vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	fclose(file);
	return true;
}

bool loadMTL(char* path, std::vector<char*>& mtl_names)
{
	int line_number = 0;
	//char current_line[OBJ_LINE_SIZE];
	char material_open = 0;
	char current_mtl_name[255];
	obj_material current_mtl = { "", "", vec3(0.2f,0.2f,0.2f), vec3(0.8f,0.8f,0.8f), vec3(1.0f,1.0f,1.0f),vec3(0),1.f, 0.f, 98.f, 1 };

	std::vector<obj_material> mtl;

	FILE* file = fopen(path, "r");

	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}
	uint cnt = 0;
	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		//skip comments
		//if (lineHeader == NULL || strcmp(lineHeader, "//") == 0 || strcmp(lineHeader, "#") == 0)
		//	continue;

		//start material
		if (strcmp(lineHeader, "newmtl") == 0)
		{
			if (cnt != 0)
				mtl.push_back(current_mtl);
			material_open = 1;
			fscanf(file, "%s\n", current_mtl_name);
			printf("current_mtl_name: %s\n", current_mtl_name);
			//strncpy(current_mtl.name, strtok(NULL, " \t"), MATERIAL_NAME_SIZE);
			mtl_names.push_back(current_mtl_name);
			strcpy(current_mtl.name, current_mtl_name);
			//mtl.push_back(current_mtl);
			cnt++;
		}

		//ambient
		else if (strcmp(lineHeader, "Ka") == 0 && material_open)
		{
			fscanf(file, "%f %f %f\n", &current_mtl.amb.x, &current_mtl.amb.y, &current_mtl.amb.z);
		}

		//diff
		else if (strcmp(lineHeader, "Kd") == 0 && material_open)
		{
			fscanf(file, "%f %f %f\n", &current_mtl.diff.x, &current_mtl.diff.y, &current_mtl.diff.z);

		}

		//specular
		else if (strcmp(lineHeader, "Ks") == 0 && material_open)
		{
			fscanf(file, "%f %f %f\n", &current_mtl.spec.x, &current_mtl.spec.y, &current_mtl.spec.z);
		}
		//shiny
		else if (strcmp(lineHeader, "Ns") == 0 && material_open)
		{
			fscanf(file, "%f\n", &current_mtl.shiny);
		}
		//transparent
		else if (strcmp(lineHeader, "Tr") == 0 && material_open)
		{
			fscanf(file, "%f\n", &current_mtl.trans);
		}
		//transmission filter
		else if (strcmp(lineHeader, "Tf") == 0 && material_open)
		{
			fscanf(file, "%f %f %f\n", &current_mtl.trans_f.x, &current_mtl.trans_f.y, &current_mtl.trans_f.z);
			//current_mtl.trans_filter.x = float(atof(strtok(NULL, " \t")));
			//current_mtl.trans_filter.y = float(atof(strtok(NULL, " \t")));
			//current_mtl.trans_filter.z = float(atof(strtok(NULL, " \t")));
		}
		// illumination type
		else if (strcmp(lineHeader, "illum") == 0 && material_open)
		{
		}

		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	for (auto& m : mtl) {
		printf("name: %s\n", m.name);
		printf("amb (%lf, %lf, %lf)\n", m.amb.x, m.amb.y, m.amb.z);
		printf("diff (%lf, %lf, %lf)\n", m.diff.x, m.diff.y, m.diff.z);
		printf("spec (%lf, %lf, %lf)\n", m.spec.x, m.spec.y, m.spec.z);
		printf("transfilter (%lf, %lf, %lf)\n", m.trans_f.x, m.trans_f.y, m.trans_f.z);
		printf("shiny %lf\n", m.shiny);
		printf("glossy %lf\n", m.glossy);
		printf("refract_idx %d\n", m.refract_index);
	}

	fclose(file);

	return true;

}
#endif
#pragma once