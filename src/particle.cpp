#include "cgmath.h"	// slee's simple math library
#include "cgut.h"	// slee's OpenGL utility
#include "particle.h"

GLuint		PARTICLE;			// vertex array for text objects
GLuint		program_particle;	// GPU program for text render
GLuint		particle_vertex_array = 0;	// ID holder for vertex array object


static const char* vert_text_path = "../bin/shaders/particle.vert";		// text vertex shaders
static const char* frag_text_path = "../bin/shaders/particle.frag";		// text fragment shaders

std::vector<particle_t> particles;


bool init_particle()
{
	program_particle = cg_create_program(vert_text_path, frag_text_path);
	if (!program_particle) return false;

	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec3(0,0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec3(1,0,0)}, {vec3(-1,1,0),vec3(0,0,1),vec3(0,1,0)}, {vec3(1,1,0),vec3(0,0,1),vec3(1,1,0)} }; // strip ordering [0, 1, 3, 2]


	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec3(0,0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec3(1,0,0)}, {vec3(-1,1,0),vec3(0,0,1),vec3(0,1,0)}, {vec3(1,1,0),vec3(0,0,1),vec3(1,1,0)} }; // strip ordering [0, 1, 3, 2]
	//particle_vertices = std::move(create_cube_vertices(8));
	//(particle_vertices, 8, 2);

	// generation of vertex buffer: use vertices as it is
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (particle_vertex_array) glDeleteVertexArrays(1, &particle_vertex_array);
	particle_vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!particle_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	PARTICLE = cg_create_vertex_array(vertex_buffer); if (!PARTICLE) { printf("%s(): VAO==nullptr\n", __func__); return false; }

	return true;
}

void render_particle()//std::string text, GLint _x, GLint _y, GLfloat scale, vec4 color, GLfloat dpi_scale)
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program_particle);

	// bind vertex array object
	glBindVertexArray(PARTICLE);

	// enable blending
	glEnable(GL_BLEND);
	for (auto& p : particles)
	{
		mat4 translate_matrix = mat4::translate(vec3(p.pos.x, p.pos.y, 0));
		mat4 scale_matrix = mat4::scale(p.scale);
		mat4 model_matrix = translate_matrix * scale_matrix;

		GLint uloc;
		uloc = glGetUniformLocation(program_particle, "color");			if (uloc > -1) glUniform4fv(uloc, 1, p.color);
		uloc = glGetUniformLocation(program_particle, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glDisable(GL_BLEND);

	// swap front and back buffers, and display to screen
}

void particle_update(ivec2 window_size, vec3 center) {
	for (auto& p : particles) p.update(center);
	
	// tricky aspect correction matrix for non-square window
	float aspect = window_size.x / float(window_size.y);
	mat4 aspect_matrix =
	{
		std::min(1 / aspect,1.0f), 0, 0, 0,
		0, std::min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}