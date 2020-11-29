#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "circle.h"		
#include "hexgrid.h"		// circle class definition
#include "cubmap.h"		// circle class definition

//*************************************
// global constants
static const char*	window_name = "cgbase - textured shading";
static const char*	vert_shader_path = "../bin/shaders/texphong.vert";
static const char*	frag_shader_path = "../bin/shaders/texphong.frag";
static const char*	mesh_vertex_path = "../bin/mesh/head.vertex.bin";	// http://graphics.cs.williams.edu/data/meshes.xml
static const char*	mesh_index_path	= "../bin/mesh/head.index.bin";		// http://graphics.cs.williams.edu/data/meshes.xml
static const char*	mesh_texture_path = "../bin/mesh/head.jpg";			// http://graphics.cs.williams.edu/data/meshes.xml

static const char* background_front_texture_path	= "../bin/mesh/textures/background_front.jpg";		
static const char* background_left_texture_path		= "../bin/mesh/textures/background_left.jpg";		
static const char* background_back_texture_path		= "../bin/mesh/textures/background_back.jpg";		
static const char* background_right_texture_path	= "../bin/mesh/textures/background_right.jpg";		
static const char* background_dawn_texture_path		= "../bin/mesh/textures/background_down.jpg";		
static const char* background_up_texture_path		= "../bin/mesh/textures/background_up.jpg";		

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0, 0, 100);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 1, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 1.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};


struct light_t
{
	vec4	position = vec4(0.0f, 0.0f, 1.0f, 0.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 0.2f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.5f, 0.5f, 0.5f, 0.5f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 0.8f);
	vec4	specular = vec4(0.5f, 0.5f, 0.5f, 0.4f);
	float	shininess = 1000.0f;
};


//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;			// ID holder for GPU program
GLuint	vertex_array = 0;		// ID holder for vertex array object
GLuint	top_vertex_array = 0;		// ID holder for vertex array object
GLuint	middle_vertex_array = 0;		// ID holder for vertex array object
GLuint	bottom_vertex_array = 0;		// ID holder for vertex array object

GLuint	cube_vertex_array = 0;		// ID holder for vertex array object
GLuint	skybox_vertex_array = 0;		// ID holder for vertex array object
GLuint	CUBE;					// texture object

//*************************************
// global variables
int		frame = 0;	// index of rendering frames
int		mode = 0;	// display mode

float	t = 0.0f;
float	cell_color_x = 0.0f;
float	cell_color_y = 0.0f;
float	cell_color_z = 0.0f;

bool	b_wireframe = false;
bool	b_rotate = true;

auto	cube	= std::move(create_cube());
auto	skybox = std::move(create_skybox());

auto	top = std::move(create_hexgrid_top());
auto	middle = std::move(create_hexgrid_middle());
auto	bottom = std::move(create_hexgrid_bottom());



bool	b_index_buffer = true;
std::vector<vertex>	unit_top_hexcell_vertices;
std::vector<vertex>	unit_middle_hexcell_vertices;
std::vector<vertex>	unit_bottom_hexcell_vertices;

std::vector<vertex>	unit_cube_vertices;
std::vector<vertex>	unit_skybox_vertices;

light_t		light;
material_t	material;
//*************************************
// scene objects
mesh*		pMesh = nullptr;
camera		cam;
trackball	tb;
//light_t		light;
//material_t	material;

//*************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar);

	// build the model matrix for oscillating scale
	//t = float(glfwGetTime());
	float scale = 1.0f + float(cos(t * 1.5f)) * 0.05f;
	mat4 model_matrix = mat4::scale(scale, scale, scale);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);

	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
	
	
	glActiveTexture(GL_TEXTURE0);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_CUBE_MAP, CUBE);
	glUniform1i(glGetUniformLocation(program, "CUBE"), 0);	 // GL_TEXTURE0


	glUniform1i(glGetUniformLocation(program, "mode"), mode);

}


void render()
{
	int i = 0;
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// notify GL that we use our own program
	glUseProgram(program);

	// bind vertex array object
	//if(pMesh&&pMesh->vertex_array) glBindVertexArray( pMesh->vertex_array );

	// render vertices: trigger shader programs to process vertex data
	//glDrawElements( GL_TRIANGLES, pMesh->index_list.size(), GL_UNSIGNED_INT, nullptr );
	
	glBindVertexArray(top_vertex_array);

	// render two circles: trigger shader program to process vertex data
	for (auto& c : top)
	{
		// per-circle update
		c.update();

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
		glUniform1i(glGetUniformLocation(program, "mode"), 1);

		//i++;
		// per-circle draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(middle_vertex_array);

	// render two circles: trigger shader program to process vertex data
	for (auto& c : middle)
	{
		// per-circle update
		c.update();

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		glUniform1i(glGetUniformLocation(program, "mode"), 1);
		//i++;
		// per-circle draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(bottom_vertex_array);

	// render two circles: trigger shader program to process vertex data
	for (auto& c : bottom)
	{
		// per-circle update
		c.update();

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		glUniform1i(glGetUniformLocation(program, "mode"), 1);
		//i++;
		// per-circle draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(cube_vertex_array);
	glDepthMask(GL_FALSE);
	cube.update();
	GLint uloc;
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cube.model_matrix);
	glUniform1i(glGetUniformLocation(program, "mode"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CUBE);

	if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);
	glDepthMask(GL_TRUE);

	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
	
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "- press 'd' to toggle display mode\n" );
	printf( "  - 0: textured shading\n" );
	printf( "  - 1: shading only\n" );
	printf( "  - 2: texture only\n" );
	printf( "  - 3: texture coordinates\n" );
	printf( "\n" );
}

std::vector<vertex> create_cube_vertices(uint N)
{
	std::vector<vertex> v;

	for (uint k = 0; k < N/2; k++)
	{
		float t = PI * 2.0f * k / float(N/2), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s, 1.0f), vec3(0,0,-1.0f), vec3(0.878431f,1.0f, 1.0f) });
	}
	for (uint k = 0; k < N/2; k++)
	{
		float t = PI * 2.0f * k / float(N/2), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s, -1.0f), vec3(0,0,-1.0f), vec3(0.878431f,1.0f, 1.0f) });
	}

	return v;
}

void update_cube_vertex_buffer(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;

		indices.push_back(0);	// the origin
		indices.push_back(1);
		indices.push_back(2);

		indices.push_back(0);	// the origin
		indices.push_back(2);
		indices.push_back(3); //2N+3
	
		indices.push_back(3);
		indices.push_back(4); //2N+3
		indices.push_back(7);	// the origin

		indices.push_back(0);	// the origin
		indices.push_back(4);
		indices.push_back(3);

		indices.push_back(0);	// the origin
		indices.push_back(4);
		indices.push_back(5); //2N+3

		indices.push_back(0);	// the origin
		indices.push_back(1);
		indices.push_back(5);

		indices.push_back(2);	// the origin
		indices.push_back(6);
		indices.push_back(5); //2N+3

		indices.push_back(1);	// the origin
		indices.push_back(2);
		indices.push_back(5);

		indices.push_back(4);	// the origin
		indices.push_back(5);
		indices.push_back(6);

		indices.push_back(4);	// the origin
		indices.push_back(6);
		indices.push_back(7); //2N+3

		indices.push_back(2);	// the origin
		indices.push_back(6);
		indices.push_back(7); //2N+3

		indices.push_back(2);	// the origin
		indices.push_back(3);
		indices.push_back(7);

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}
	else
	{
		std::vector<vertex> v; // triangle vertices

		v.push_back(vertices[0]);	// the origin
		v.push_back(vertices[1]);
		v.push_back(vertices[2]);

		v.push_back(vertices[0]);	// the origin
		v.push_back(vertices[2]);
		v.push_back(vertices[3]);

		v.push_back(vertices[7]);
		v.push_back(vertices[3]);
		v.push_back(vertices[4]);

		v.push_back(vertices[0]);	// the origin
		v.push_back(vertices[4]);
		v.push_back(vertices[3]);

		v.push_back(vertices[0]);
		v.push_back(vertices[4]);
		v.push_back(vertices[1]);

		v.push_back(vertices[0]);
		v.push_back(vertices[1]);
		v.push_back(vertices[5]);

		v.push_back(vertices[2]);	// the origin
		v.push_back(vertices[6]);
		v.push_back(vertices[5]);

		v.push_back(vertices[1]);	// the origin
		v.push_back(vertices[2]);
		v.push_back(vertices[5]);

		v.push_back(vertices[4]);	// the origin
		v.push_back(vertices[5]);
		v.push_back(vertices[6]);

		v.push_back(vertices[4]);
		v.push_back(vertices[6]);
		v.push_back(vertices[7]);

		v.push_back(vertices[2]);
		v.push_back(vertices[6]);
		v.push_back(vertices[7]);

		v.push_back(vertices[2]);
		v.push_back(vertices[3]);
		v.push_back(vertices[7]);

		// generation of vertex buffer: use triangle_vertices instead of vertices
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (cube_vertex_array) glDeleteVertexArrays(1, &cube_vertex_array);
	cube_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!cube_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_hexcell_vertices(uint N)
{
	std::vector<vertex> v = { { vec3(0,0,0.2f), vec3(0,0,1.0f), vec3(cell_color_x, cell_color_y, cell_color_z) } }; // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N) + PI * 1.0f * 1 / float(6), c = 0.9f * cos(t), s = 0.9f * sin(t);
		v.push_back({ vec3(c,s,0.2f), vec3(c,s,0), vec3(cell_color_x, cell_color_y, cell_color_z) });
	}
	v.push_back({ vec3(0,0,-0.2f), vec3(0,0,-1.0f), vec3(cell_color_x, cell_color_y, cell_color_z) }); // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N) + PI * 1.0f * 1 / float(6), c = 0.9f * cos(t), s = 0.9f * sin(t);
		v.push_back({ vec3(c,s,-0.2f), vec3(c,s,0), vec3(cell_color_x, cell_color_y, cell_color_z) });
	}
	v.push_back({ vec3(0,0,0), vec3(0,0,1.0f), vec3(cell_color_x, cell_color_y, cell_color_z) }); // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N) + PI * 1.0f * 1 / float(6), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s,0), vec3(c,s,0), vec3(cell_color_x, cell_color_y, cell_color_z) + 0.3f });
	}
	return v;
}

void update_hexcell_vertex_buffer(const std::vector<vertex>& vertices, uint N, uint flag)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;
		for (uint k = 0; k < N; k++)
		{
			indices.push_back(0);	// the origin
			indices.push_back(k + 1);
			indices.push_back(k + 2);
			
			indices.push_back(N+2);	// the origin
			indices.push_back(N+2 + k + 1);
			indices.push_back(N+2 + k + 2); //2N+3

			indices.push_back(2*N + 4);	// the origin
			indices.push_back(2 * N + 4 + k + 1);
			indices.push_back(2 * N + 4 + k + 2); //2N+3
			
			indices.push_back(k + 1);
			indices.push_back(k + 2);
			indices.push_back(2*N+5 + k + 1);

			indices.push_back(2*N+5+k);
			indices.push_back(k + 1);
			indices.push_back(2*N+5 + k + 1);

			indices.push_back(N + 2 + k + 1);
			indices.push_back(N + 2 + k + 2);
			indices.push_back(2 * N + 5 + k + 1);

			indices.push_back(2 * N + 5 + k);
			indices.push_back(N + 2 + k + 1);
			indices.push_back(2 * N + 5 + k + 1);

		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}
	else
	{
		std::vector<vertex> v; // triangle vertices
		for (uint k = 0; k < N; k++)
		{
			v.push_back(vertices[0]);	// the origin
			v.push_back(vertices[k + 1]);
			v.push_back(vertices[k + 2]);
			
			v.push_back(vertices[N+2]);	// the origin
			v.push_back(vertices[N+2 + k + 1]);
			v.push_back(vertices[N+2 + k + 2]);

			v.push_back(vertices[2 * N + 4]);	// the origin
			v.push_back(vertices[2 * N + 4 + k + 1]);
			v.push_back(vertices[2 * N + 4 + k + 2]);

			v.push_back(vertices[k + 1]);
			v.push_back(vertices[k + 2]);
			v.push_back(vertices[2*N+5 + k + 1]);

			v.push_back(vertices[2*N+5 + k]);
			v.push_back(vertices[k + 1]);
			v.push_back(vertices[2*N+5 + k + 1]);

			v.push_back(vertices[N+2 + k + 1]);
			v.push_back(vertices[N+2 + k + 2]);
			v.push_back(vertices[2*N+5 + k + 1]);

			v.push_back(vertices[2*N+5 + k]);
			v.push_back(vertices[N+2+k + 1]);
			v.push_back(vertices[2*N+5 + k + 1]);
		}

		// generation of vertex buffer: use triangle_vertices instead of vertices
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}


	if (flag == 0) {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (top_vertex_array) glDeleteVertexArrays(1, &top_vertex_array);
		top_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!top_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
	else if (flag == 1) {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (middle_vertex_array) glDeleteVertexArrays(1, &middle_vertex_array);
		middle_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!middle_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
	else {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (bottom_vertex_array) glDeleteVertexArrays(1, &bottom_vertex_array);
		bottom_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!bottom_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end();
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_SHIFT))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_zooming(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_zooming();
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_panning(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_panning();
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking() && !tb.is_zooming() && !tb.is_panning()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	if (tb.is_tracking())cam.view_matrix = tb.update(npos);
	if (tb.is_zooming())cam.view_matrix = tb.update_zooming(npos);
	if (tb.is_panning())cam.view_matrix = tb.update_panning(npos);
}

GLuint create_texture(const char* image_path, bool b_mipmap)
{
	// load the image with vertical flipping
	image* img = cg_load_image(image_path); if (!img) return -1;
	int w = img->width, h = img->height;

	// create a src texture (lena texture)
	GLuint texture; glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	if (img) delete img; // release image

	// build mipmap
	if (b_mipmap && glGenerateMipmap)
	{
		int mip_levels = 0; for (int k = std::max(w, h); k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, GL_RGB8, std::max(1, w >> l), std::max(1, h >> l), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, b_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	return texture;
}

//create_texture
GLuint create_cubemap()
{	
	GLuint texture; glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	image* img = cg_load_image(background_front_texture_path); if (!img) return -1;
	int w = img->width, h = img->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	if (img) delete img; // release image
	
	image* img1 = cg_load_image(background_left_texture_path); if (!img1) return -1;
	int w1 = img1->width, h1 = img1->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, w1, h1, 0, GL_RGB, GL_UNSIGNED_BYTE, img1->ptr);
	if (img1) delete img1; // release image

	image* img2 = cg_load_image(background_back_texture_path); if (!img2) return -1;
	int w2 = img2->width, h2 = img2->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, w2, h2, 0, GL_RGB, GL_UNSIGNED_BYTE, img2->ptr);
	if (img2) delete img2; // release image

	image* img3 = cg_load_image(background_right_texture_path); if (!img3) return -1;
	int w3 = img3->width, h3 = img3->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, w3, h3, 0, GL_RGB, GL_UNSIGNED_BYTE, img3->ptr);
	if (img3) delete img3; // release image

	image* img4 = cg_load_image(background_dawn_texture_path); if (!img4) return -1;
	int w4 = img4->width, h4 = img4->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, w4, h4, 0, GL_RGB, GL_UNSIGNED_BYTE, img4->ptr);
	if (img4) delete img4; // release image

	image* img5 = cg_load_image(background_up_texture_path); if (!img5) return -1;
	int w5 = img5->width, h5 = img5->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, w5, h5, 0, GL_RGB, GL_UNSIGNED_BYTE, img5->ptr);
	if (img5) delete img5; // release image

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}
//GL_TEXTURE_CUBE_MAP_POSITIVE_X
//GL_TEXTURE_CUBE_MAP_NEGATIVE_X	
//GL_TEXTURE_CUBE_MAP_POSITIVE_Y
//GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	
//GL_TEXTURE_CUBE_MAP_POSITIVE_Z	
//GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	//glEnable(GL_CULL_FACE);									// turn on backface culling
	glEnable(GL_DEPTH_TEST);									// turn on depth tests
	glEnable(GL_TEXTURE_CUBE_MAP);								// enable texturing
	glEnable(GL_TEXTURE_2D);									// enable texturing
	glActiveTexture(GL_TEXTURE0);								// notify GL the current texture slot is 0


	cell_color_x = 0.878431f;
	cell_color_y = 1.0f;
	cell_color_z = 1.0f;
	unit_top_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_top_hexcell_vertices, 6, 0);

	cell_color_x = 0.196078f;
	cell_color_y = 0.803922f;
	cell_color_z = 1.0f;
	unit_middle_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_middle_hexcell_vertices, 6, 1);


	cell_color_x = 1.0f;
	cell_color_y = 0.713725f;
	cell_color_z = 0.756863f;
	unit_bottom_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_bottom_hexcell_vertices, 6, 2);

	unit_cube_vertices = std::move(create_cube_vertices(8));
	update_cube_vertex_buffer(unit_cube_vertices, 8);

	CUBE = create_cubemap();			if (CUBE == -1) return false;

	return true;

}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}

