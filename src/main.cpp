#include "cgmath.h"				// slee's simple math library
#include "cgut.h"				// slee's OpenGL utility
#include "trackball.h"			// virtual trackball
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "circle.h"		
#include "hexgrid.h"			// circle class definition
#include "cubmap.h"				// circle class definition
#include "collisiondection.h"
#include "npc.h"
#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
#include "loadobj.h"
#include "kalbi.h"

#define ANGLE_Y 1000.0f
#define ANGLE_Z 2000.0f

//*******************************************************************
// forward declarations for freetype text
bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);
void gamestart();

//*************************************
// global constants
static const char* window_name = "cgbase - textured shading";
static const char* vert_shader_path = "../bin/shaders/texphong.vert";
static const char* frag_shader_path = "../bin/shaders/texphong.frag";

static const char* vert_shader_path2 = "../bin/shaders/description.vert";
static const char* frag_shader_path2 = "../bin/shaders/description.frag";

static const char* hell_front_texture_path = "../bin/textures/hell_front.jpg";
static const char* hell_left_texture_path = "../bin/textures/hell_right.jpg";
static const char* hell_back_texture_path = "../bin/textures/hell_back.jpg";
static const char* hell_right_texture_path = "../bin/textures/hell_left.jpg";
static const char* hell_dawn_texture_path = "../bin/textures/hell_up.jpg";
static const char* hell_up_texture_path = "../bin/textures/hell_down.jpg";

static const char* background_front_texture_path = "../bin/textures/background_front.jpg";
static const char* background_left_texture_path = "../bin/textures/background_left.jpg";
static const char* background_back_texture_path = "../bin/textures/background_back.jpg";
static const char* background_right_texture_path = "../bin/textures/background_right.jpg";
static const char* background_dawn_texture_path = "../bin/textures/background_down.jpg";
static const char* background_up_texture_path = "../bin/textures/background_up.jpg";

static const char* description1_texture_path = "../bin/textures/background_right.jpg";
static const char* description2_texture_path = "../bin/textures/background_down.jpg";
static const char* description3_texture_path = "../bin/textures/background_up.jpg";


static const char* title_obj_path = "../bin/mesh/gokalbi.obj";
static const char* player_obj_path = "../bin/model/player_all.obj";
static const char* npc_obj_path = "../bin/model/npc_all.obj";
static const char* kalbi_obj_path = "../bin/model/galbi2.obj";

static const char* easy_bgm_path = "../bin/sound/bgm.mp3";
static const char* hell_bgm_path = "../bin/sound/hell_bgm.mp3";
static const char* jump_bgm_path = "../bin/sound/JumpingSound.mp3";
static const char* hit_bgm_path = "../bin/sound/smb3_kick.wav";
static const char* ready_bgm_path = "../bin/sound/mk64_countdown_set.mp3";
static const char* go_bgm_path = "../bin/sound/mk64_countdown_go.mp3";





//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0, -5000, 0);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 0, 1);
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

// flags of keys for smooth changes
struct b_loc {
	bool right = false, left = false, front = false, back = false, jump = false;
	operator bool() const { return left || right || front || back; }
} b_user;

//*************************************
// window objects
GLFWwindow* window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size
int			defaultz_xsize = window_size.x;
int			defaultz_ysize = window_size.y;

//*************************************
// OpenGL objects
GLuint	program = 0;				// ID holder for GPU program
GLuint	program2 = 0;				// ID holder for GPU program

GLuint	vertex_array = 0;			// ID holder for vertex array object
GLuint	top_vertex_array = 0;		// ID holder for vertex array object - hexgrid on the top 
GLuint	middle_vertex_array = 0;	// ID holder for vertex array object - hexgrid on the middle
GLuint	bottom_vertex_array = 0;	// ID holder for vertex array object - hexgrid on the bottom
GLuint	easymode_vertex_array = 0;		// ID holder for vertex array object - cube map
GLuint	EASYMODE;						// texture object					 - cube map

GLuint	hellmode_vertex_array = 0;		// ID holder for vertex array object - cube map
GLuint	HELLMODE;						// texture object					 - cube map

GLuint  description1_texture;
GLuint  description2_texture;
GLuint  description3_texture;

GLuint	test_vertex_array = 0;		// for test( ------------------------------- delete later ---------------------------- )
GLuint  title_vertex_array = 0;		// ID holder for vertex array object
GLuint  title_index_array = 0;		// ID holder for index buffer
GLuint  title_texture;
GLuint  player_vertex_array = 0;		// ID holder for vertex array object
GLuint  player_left_vertex_array = 0;		// ID holder for vertex array object
GLuint  npc_vertex_array = 0;		// ID holder for vertex array object
GLuint  npc_leg_vertex_array = 0;		// ID holder for vertex array object
GLuint  kalbi_vertex_array = 0;		// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;	// index of rendering frames
int		mode = 0;	// display mode
int		mode2 = 0;	// display mode

int		level = 0;	// multilevel difficulty

float	t = 0.0f;
float	t_check = 0.0f;
float	t_ready = 0.0f;
float	t_jump = 0.0f;
float	z_jump_top = 0.0f;
float	z_jump_down = 0.0f;
float	cell_color_x = 0.0f;
float	cell_color_y = 0.0f;
float	cell_color_z = 0.0f;
float	user_forward = 0.0f;							// forward direction of the user
uint	countdown_cnt = 0;
uint	num_npc_left = 0;

auto	easy_mode = std::move(create_cube());
auto	hell_mode = std::move(create_cube());

auto	descriptions = std::move(create_description());
auto	top = std::move(create_hexgrid_top());
auto	middle = std::move(create_hexgrid_middle());
auto	bottom = std::move(create_hexgrid_bottom());
auto	test = std::move(create_test_obj());			// for test( ------------------------------- delete later ---------------------------- )
std::vector<NPC> npc;
auto	kalbis = std::move(create_kalbis());

bool	b_wireframe = false;
bool	b_rotate = true;
bool	b_index_buffer = true;
bool	b_started = false;
bool	b_starting_zoom = false;
bool	b_ready = false;
bool	b_won = false;
bool	b_restart = false;
bool	b_description = false;
bool	b_jump_start = false;
bool	b_jump_top = false;

std::vector<vertex>	unit_desc_hexcell_vertices;
std::vector<vertex>	unit_top_hexcell_vertices;
std::vector<vertex>	unit_middle_hexcell_vertices;
std::vector<vertex>	unit_bottom_hexcell_vertices;
std::vector<vertex>	test_vertices;						// for test( ------------------------------- delete later ---------------------------- )
std::vector<vertex>	unit_easymode_vertices;
std::vector<vertex>	unit_hellmode_vertices;
std::vector<vertex>	title_vertices;						// holder of vertices and indices of title obj
std::vector<vertex>	player_vertices;						// holder of vertices and indices of title obj
std::vector<vertex>	npc_vertices;						// holder of vertices and indices of title obj
std::vector<vertex>	kalbi_vertices;						// holder of vertices and indices of title obj
std::vector<vertex>	particle_vertices;						// holder of vertices and indices of title obj


vec3	user_loc = vec3(0, 0, 21.2f);

//*************************************
// scene objects
mesh* pMesh = nullptr;
camera		cam;
trackball	tb;
light_t		light;
material_t	material;


//*************************************
//irrKlang objects
irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* bgm = nullptr;
irrklang::ISoundSource* readysound = nullptr;
irrklang::ISoundSource* gosound = nullptr;
irrklang::ISoundSource* jumpsound = nullptr;
irrklang::ISoundSource* hitsound = nullptr;

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

	gamestart();

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


	glActiveTexture(GL_TEXTURE6);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_CUBE_MAP, EASYMODE);
	glUniform1i(glGetUniformLocation(program, "EASYMODE"), 0);	 // GL_TEXTURE0

	glActiveTexture(GL_TEXTURE4);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_CUBE_MAP, HELLMODE);
	glUniform1i(glGetUniformLocation(program, "HELLMODE"), 4);	 // GL_TEXTURE0

	glActiveTexture(GL_TEXTURE10);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, description1_texture);
	glUniform1i(glGetUniformLocation(program2, "description1_texture"), 5);	 // GL_TEXTURE0

	glActiveTexture(GL_TEXTURE11);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, description2_texture);
	glUniform1i(glGetUniformLocation(program2, "description2_texture"), 6);	 // GL_TEXTURE0

	glActiveTexture(GL_TEXTURE12);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, description3_texture);
	glUniform1i(glGetUniformLocation(program2, "description3_texture"), 7);	 // GL_TEXTURE0

	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glUniform1i(glGetUniformLocation(program2, "mode2"), mode2);


}
void render()
{
	int middleX = GetSystemMetrics(SM_CXSCREEN) >> 1;
	int middleY = GetSystemMetrics(SM_CYSCREEN) >> 1;
	int i = 0;
	// clear screen (with background color) and clear depth buffer
	glClearColor(0.76f, 1.0f, 0.8f, 1.0f);	// set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (b_won) {
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float dpi_scale = cg_get_dpi_scale();
		render_text("You Win!!", window_size.x / 5, 1 * window_size.y / 3, 0.8f * window_size.x / defaultz_xsize, vec4(0.0f, 0.7f, 0.3f, 1.0f), dpi_scale);
		render_text("Press R to go first page", window_size.x / 5, 1 * window_size.y / 3 + 50, 0.5f * window_size.x / defaultz_xsize, vec4(0.0f, 0.4f, 0.3f, 1.0f), dpi_scale);
		//glBlendFunc(GL_ONE, GL_ZERO);
	}
	else if (b_description) {
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float dpi_scale = cg_get_dpi_scale();
		render_text("move: w(front), a(left), s(right), d(back)", window_size.x / 5, 1 * window_size.y / 3, 0.4f * window_size.x / defaultz_xsize, vec4(0.5f, 0.6f, 0.2f, 1.0f), dpi_scale);
		render_text("mouse: if you move a mouse to left-side,", window_size.x / 5, 1 * window_size.y / 3 + 50, 0.4f * window_size.x / defaultz_xsize, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
		render_text("screen also rotate to left-side and vice versa", window_size.x / 5, 1 * window_size.y / 3 + 100, 0.4f * window_size.x / defaultz_xsize, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
		render_text("Press R or F1 to go first page", window_size.x / 5, 1 * window_size.y / 3 + 150, 0.4f * window_size.x / defaultz_xsize, vec4(0.7f, 0.4f, 0.1f, 0.8f), dpi_scale);
		//glBlendFunc(GL_ONE, GL_ZERO);
	}
	else if (b_restart) {
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float dpi_scale = cg_get_dpi_scale();
		render_text("Gameover!!", window_size.x / 5, 1 * window_size.y / 3, 0.8f * window_size.x / defaultz_xsize, vec4(0.3f, 0.46f, 0.4f, 0.4f), dpi_scale);
		render_text("Press R to go first page", window_size.x / 5, 1 * window_size.y / 3 + 50, 0.5f * window_size.x / defaultz_xsize, vec4(0.0f, 0.4f, 0.3f, 1.0f), dpi_scale);
		//glBlendFunc(GL_ONE, GL_ZERO);
	}
	else if (!b_starting_zoom && !b_ready) {
		// render texts
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float dpi_scale = cg_get_dpi_scale();
		render_text("Press 1(easy mode), 2(hard mode), 3(hell mode) to start!!", window_size.x / 5, 2 * window_size.y / 3, 0.3f * window_size.x / defaultz_xsize, vec4(0.5f, 0.f, 0.6f, 0.8f), dpi_scale);
		render_text("Press F1 to see description", window_size.x / 5, 2 * window_size.y / 3 + 50, 0.3f * window_size.x / defaultz_xsize, vec4(0.9f, 0.5f, 0.0f, 0.9f), dpi_scale);
		//glBlendFunc(GL_ONE, GL_ZERO);
	}
	else {
		if (level == 3) {
			glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
			float dpi_scale = cg_get_dpi_scale();
			render_text("HP: " + std::to_string(int(test.HP)), 13 * window_size.x / 20, 50, 0.5f * window_size.x / defaultz_xsize, vec4(0.7f, 0.4f, 0.1f, 1.0f), dpi_scale);
			//glBlendFunc(GL_ONE, GL_ZERO);
		}
		else {
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(0.0f, 0.0f, 0.f, 1.0f);	// set clear color
			float dpi_scale = cg_get_dpi_scale();
			render_text("HP: " + std::to_string(int(test.HP)), 13 * window_size.x / 20, 50, 0.5f * window_size.x / defaultz_xsize, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
			//glBlendFunc(GL_ONE, GL_ZERO);
		}
	}
	// notify GL that we use our own program
	glUseProgram(program2);

	/**************** description ******************/
	if (b_description) {
		i = 0;
		glBindVertexArray(vertex_array);
		// render two circles: trigger shader program to process vertex data
		for (auto& c : descriptions)
		{
			// per-circle update
			c.update();

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program2, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
			glUniform1i(glGetUniformLocation(program2, "mode2"), 5 + i);
			i++;
			//i++;
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
			// per-circle draw calls
		}
		i = 0;
	}
	glUseProgram(program);

	/**************** player ****************/
	glBindVertexArray(player_vertex_array);

	// render two circles: trigger shader program to process vertex data
	uint t_index = 0;
	uint m_index = 0;
	uint b_index = 0;
	bool flag = false;

	if (b_started && !b_starting_zoom) {
		if (test.top_collide(top)) {
			top = after_collide(test, top);
		}
		else if (test.middle_collide(middle)) {
			middle = after_collide(test, middle);
		}
		else if (test.bottom_collide(bottom)) {
			bottom = after_collide(test, bottom);
		}
		for (auto& c : top) {
			if (glfwGetTime() - c.time > 0.8f && c.time != 0.0f) {
				top = dispear(top, t_index);
				break;
			}
			t_index++;
		}
		for (auto& c : middle) {
			if (glfwGetTime() - c.time > 0.8f && c.time != 0.0f) {
				middle = dispear(middle, m_index);
				break;
			}
			m_index++;
		}
		for (auto& c : bottom) {
			if (glfwGetTime() - c.time > 0.8f && c.time != 0.0f) {
				//printf("bottom: %f %f\n", c.center.x, c.center.y);
				bottom = dispear(bottom, b_index);
				break;
			}
			b_index++;
		}
		float cost = cosf(user_forward), sint = sinf(user_forward);
		if (b_user.left) {
			test.center.x -= 0.003f * cost;
			test.center.y -= 0.003f * sint;
			test.theta = PI / 2 + user_forward;
		}
		if (b_user.right) {
			test.center.x += 0.003f * cost;
			test.center.y += 0.003f * sint;
			test.theta = -1 * PI / 2 + user_forward;
		}
		if (b_user.front) {
			test.center.x -= 0.003f * sint;
			test.center.y += 0.003f * cost;
			test.theta = 0.0f + user_forward;
		}
		if (b_user.back) {
			test.center.x += 0.003f * sint;
			test.center.y -= 0.003f * cost;
			test.theta = PI + user_forward;

		}
		if (b_user.jump) {
			float origin_gravity = test.gravity;
			test.gravity = 0.0f;
			if (b_jump_start) {
				if (test.center.z + 1.0f >= 21.2f && test.center.z + 1.0f <= 22.5f) { z_jump_top = test.center.z + 1.0f; z_jump_down = 21.2f; }
				else if (test.center.z + 1.0f >= 1.2f && test.center.z + 1.0f <= 2.5f) { z_jump_top = test.center.z + 1.0f; z_jump_down = 1.2f; }
				else if (test.center.z + 1.0f >= -18.8f && test.center.z + 1.0f <= -17.5f) { z_jump_top = test.center.z + 1.0f; z_jump_down = -18.8f; }
				else { flag = true; test.gravity = origin_gravity; }
				b_jump_start = false;
			}
			if (!flag) {
				if (!b_jump_top) {
					if (test.center.z < z_jump_top) {
						test.center.z += 0.002f;
					}
					else
						b_jump_top = true;
				}

				else if (b_jump_top) {
					if (test.center.z > z_jump_down) {
						test.center.z -= 0.002f;
					}
					else {
						b_jump_top = false;
						b_user.jump = false;
					}
				}
				else b_user.jump = false;
			}
			else {
				b_user.jump = false;
				b_jump_top = false;
			}
		}

		user_loc = test.center;

	}
	test.update(t, b_started);
	// update per-circle uniforms
	GLint uloc;
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, test.model_matrix);
	glUniform1i(glGetUniformLocation(program, "mode"), 1);
	//i++;
	// per-circle draw calls
	glBindVertexArray(player_vertex_array);
	glDrawElements(GL_TRIANGLES, player_vertices.size(), GL_UNSIGNED_INT, nullptr);

	/*************** kalbi *****************/
	glBindVertexArray(kalbi_vertex_array);	// later change to npc vertices
	// kalbi shoot
	//if (b_kalbi_shoot) {
	for (auto& k : kalbis) {

		//printf("here!");
		if (k.used == true) {

			//	k_now = create_kalbi(test);
			//	b_know_exist = true;
			//}
			//printf("t(%lf, %lf, %lf) s(%lf, %lf, %lf)\n", test.center.x, test.center.y, test.center.z, k_now.shoot_point.x, k_now.shoot_point.y, k_now.shoot_point.z);
			float t_sec = float(glfwGetTime()) - k.t_shoot;
			//if (kalbis.size() < 0)
			//	kalbis = load_kalbis(test);
	//		kalbis.erase(vec_test.begin());
			float lx_t = k.veloticy * cosf(k.throw_angle) * t_sec;
			float ly_t = k.veloticy * sinf(k.throw_angle) * t_sec - k.gravity * float(pow(t_sec, 2)) / 2;

			k.center.x = k.shoot_point.x - lx_t * sinf(user_forward);
			k.center.y = k.shoot_point.y + lx_t * cosf(user_forward);
			k.center.z = k.shoot_point.z + ly_t;

			//printf("t(%lf, %lf, %lf) k(%lf, %lf, %lf)\n", test.center.x, test.center.y, test.center.z, k.center.x, k.center.y, k.center.z);
			k.update(t);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, k.model_matrix);
			glUniform1i(glGetUniformLocation(program, "mode"), 1);

			// per-circle draw calls
			//if (b_index_buffer)	glDrawElements(GL_TRIANGLES, kalbi_vertices.size(), GL_UNSIGNED_INT, nullptr);
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 36 * 72 * 3, GL_UNSIGNED_INT, nullptr);
			else  glDrawArrays(GL_TRIANGLES, 0, kalbi_vertices.size());
		}
	}

	/**************** npc ******************/
	glBindVertexArray(npc_vertex_array);	// later change to npc vertices
	uint cnt = 0;
	// render two circles: trigger shader program to process vertex data
	for (auto& n : npc)
	{
		//printf("id:%d ",n.id);
		if (n.HP > 0) {
			cnt++;
			if (n.center.z > test.center.z + 10.f && test.center.z > -20.f)
				n.center.z -= 20.f;
			//printf("alive! HP: %lf \n",n.HP);
			if (b_started && !b_starting_zoom) {
				test.if_collide_npc(n);
				for (auto& k : kalbis) {
					if (k.used == true) {
						//k_now = k;
						bool b_kalbi_hit = k.if_collide_npc(n, level);
						if (b_kalbi_hit) {
							k.used = false;
						}
						if (k.center.z < (k.shoot_point.z - 10.f)) {
							k.used = false;
						}
					}
				}
			}
			//	printf("%f\n", test.HP);w2
			n.if_collide_other(npc);
			float cost = cosf(user_forward), sint = sinf(user_forward);
			// move to 4 direction

			n.update(t);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, n.model_matrix);
			glUniform1i(glGetUniformLocation(program, "mode"), 1);
			//i++;
			// per-circle draw calls
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, npc_vertices.size(), GL_UNSIGNED_INT, nullptr);
		}
		else {
			//printf("dead.\n");
		}
	}
	num_npc_left = cnt;
	if_overwall(npc);

	/**************** map ******************/
	uint tmp = 0;
	glDisable(GL_CULL_FACE);
	glBindVertexArray(top_vertex_array);
	// render two circles: trigger shader program to process vertex data
	for (auto& c : top)
	{
		if (c.time > 0.0f) {
			t = (float(glfwGetTime()) - c.time) * 0.001f;
			c.color.x -= t;

			c.update();
			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
			uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
			glUniform1i(glGetUniformLocation(program, "mode"), 2);

			//i++;
			// per-circle draw calls
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
		}
		// per-circle update
		c.update();

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
		glUniform1i(glGetUniformLocation(program, "mode"), 1);

		//i++;
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
		// per-circle draw calls
	}

	glBindVertexArray(middle_vertex_array);

	// render two circles: trigger shader program to process vertex data
	for (auto& c : middle)
	{
		if (c.time > 0.0f) {
			t = (float(glfwGetTime()) - c.time) * 0.00006f;
			c.color.x += t;
			c.color.y += t;
			c.color.z -= t * 10.0f;
			c.update();

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
			uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
			glUniform1i(glGetUniformLocation(program, "mode"), 2);
			//i++;
			// per-circle draw calls
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
		}
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
		if (c.time > 0.0f) {
			t = (float(glfwGetTime()) - c.time) * 0.0003f;
			c.color.y -= t;
			c.color.z -= t;

			c.update();

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
			uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
			glUniform1i(glGetUniformLocation(program, "mode"), 2);
			//i++;
			// per-circle draw calls
			if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 48 * 3, GL_UNSIGNED_INT, nullptr);
		}
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

	if (level == 1 || level == 2) {
		glBindVertexArray(easymode_vertex_array);
		glDepthMask(GL_FALSE);
		easy_mode.update();
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, easy_mode.model_matrix);
		glUniform1i(glGetUniformLocation(program, "mode"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, EASYMODE);


		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);
	}
	else if (level == 3) {
		glBindVertexArray(hellmode_vertex_array);
		glDepthMask(GL_FALSE);
		hell_mode.update();
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, hell_mode.model_matrix);
		glUniform1i(glGetUniformLocation(program, "mode"), 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, HELLMODE);

		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);
	}
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	/*************** title *****************/
	glBindVertexArray(title_vertex_array);
	glUniform1i(glGetUniformLocation(program, "mode"), 1);
	glBindTexture(GL_TEXTURE_2D, title_texture);
	mat4 model_matrix;
	if (!b_starting_zoom && !b_started && !b_ready)
		/*mat4 */model_matrix = mat4::translate(0, -4900, 0);
	else
		model_matrix = mat4::translate(0, -75, 0);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	glDrawElements(GL_TRIANGLES, title_vertices.size() * 12, GL_UNSIGNED_INT, nullptr);

	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
}

void gamestart() {
	// start game with zoom-in
	if (b_won) {
		cam.eye = vec3(0.0f, -15000.0f, 0.0f);
		cam.at = vec3(0.0f, 0.0f, 1.0f);
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		return;
	}
	if (b_starting_zoom) {
		if (cam.eye.y < -8 && cam.eye.z < 44 && cam.at.z < 40) {
			cam.eye.y += (float(glfwGetTime()) - t_check) * 0.0999f;
			cam.eye.z += (float(glfwGetTime()) - t_check) * 0.025f;
			cam.at.z += (float(glfwGetTime()) - t_check) * 0.025f;

			//printf("eye (%lf, %lf, %lf)\n at(%lf, %lf, %lf) \n up(%lf, %lf, %lf)\n", cam.eye.x, cam.eye.y, cam.eye.z, cam.at.x, cam.at.y, cam.at.z, cam.up.x, cam.up.y, cam.up.z);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		}
		else {
			b_starting_zoom = false;
			t_ready = float(glfwGetTime());
			b_ready = true;
			//b_started = true;
		}
	}
	else if (!b_started && b_ready) {
		float countdown = float(glfwGetTime()) - t_ready;
		if (countdown_cnt == 0) {
			printf("3\n");
			countdown_cnt++;
			engine->play2D(readysound, false);
			readysound->setDefaultVolume(1.f);
			while (glfwGetTime() - t_ready < 1.f) {

			}
		}
		else if (countdown_cnt == 1) {
			printf("2\n");
			countdown_cnt++;
			engine->play2D(readysound, false);
			//readysound->setDefaultVolume(0.6f);
			while (glfwGetTime() - t_ready < 2.f) {

			}
		}
		else if (countdown_cnt == 2) {
			printf("1\n");
			countdown_cnt++;
			engine->play2D(readysound, false);
			//readysound->setDefaultVolume(0.6f);
			while (glfwGetTime() - t_ready < 3.f) {

			}
		}
		else {
			printf("GO!\n");
			countdown_cnt = 0;
			engine->play2D(gosound, false);
			gosound->setDefaultVolume(1.f);
			b_started = true;
		}
	}
	// game has started!
	if (b_started && !b_starting_zoom) {
		cam.eye.x = user_loc.x + 8 * sinf(user_forward);
		cam.eye.y = user_loc.y - 8 * cosf(user_forward);
		cam.eye.z = user_loc.z + 4;
		cam.at = user_loc;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		//cam.view_matrix = mat4::look_at(user_loc.x, user_loc.y, );
	}
	if (test.center.z < -40.0f || test.HP < 0) {
		cam.eye = vec3(0.0f, -10000.0f, 0.0f);
		cam.at = vec3(0.0f, 0.0f, 1.0f);
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		test.center.x = 0.0f;
		test.center.y = 0.0f;
		test.center.z = 40.0f;
		npc.clear();
		test.center = vec3(0.f, 0.f, 40.f);
		b_starting_zoom = false;
		b_started = false;
		b_ready = false;
		b_restart = true;
		b_won = false;
		top = std::move(create_hexgrid_top());
		middle = std::move(create_hexgrid_middle());
		bottom = std::move(create_hexgrid_bottom());
		test.HP = 500.0f;
	}
	if (b_started && !b_starting_zoom && num_npc_left == 0) {
		engine->stopAllSounds();
		b_started = false;
		b_ready = false;
		b_starting_zoom = false;
		b_description = false;
		npc.clear();
		test.center = vec3(0.f, 0.f, 40.f);
		b_won = true;
		cam.eye = vec3(0.0f, -15000.0f, 0.0f);
		cam.at = vec3(0.0f, 0.0f, 1.0f);
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		top = std::move(create_hexgrid_top());
		middle = std::move(create_hexgrid_middle());
		bottom = std::move(create_hexgrid_bottom());
		test.HP = 500.0f;
		//if(level == 3)
		//	printf("You WIN!");
		//else if (level == 1) {
		//	if (num_npc_left == 0) {
		//		b_started = false;
		//		b_ready = true;
		//		b_starting_zoom = false;
		//		npc.clear();
		//		level = 2;
		//		npc = std::move(create_NPCs(level));
		//		test.center = vec3(0.f, 0.f, 40.f);
		//		t_check = float(glfwGetTime());
		//	}
		//}
		//else if (level == 2) {
		//	b_started = false;
		//	b_ready = true;
		//	b_starting_zoom = false;
		//	npc.clear();
		//	level = 3;
		//	npc=std::move(create_NPCs(level));
		//	test.center = vec3(0.f, 0.f, 40.f);
		//	t_check = float(glfwGetTime());
		//	engine->removeSoundSource(easy_bgm_path);
		//	bgm = engine->addSoundSourceFromFile(hell_bgm_path);
		//	//play sound file
		//	engine->play2D(bgm, true);
		//	bgm->setDefaultVolume(0.6f);
		//}
	}
}

void shoot_kalbi() {
	for (auto& k : kalbis) {
		if (k.used == false) {
			k.shoot_point = vec3(test.center.x, test.center.y, test.center.z + 2.f);
			k.used = true;
			k.t_shoot = float(glfwGetTime());
			return;
		}
	}
	return;
}

void reshape(GLFWwindow* window, int width, int height)
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press Home to reset camera\n");
	printf("- press 'd' to toggle display mode\n");
	printf("  - 0: textured shading\n");
	printf("  - 1: shading only\n");
	printf("  - 2: texture only\n");
	printf("  - 3: texture coordinates\n");
	printf("\n");
}


std::vector<vertex> create_test_vertices()
{
	std::vector<vertex> v = { { vec3(0), vec3(0,0,-1.0f), vec3(0.5f) } }; // origin
	float cosine, sine, t;
	float p, p_cosine, p_sine;
	for (uint k = 0; k <= 36; k++)
	{
		t = PI * 2.0f * float(k) / float(36);
		cosine = cos(t);
		sine = sin(t);
		for (uint l = 0; l <= 72; l++) {
			p = PI * 2.0f * (float(l) / float(72));
			p_cosine = cos(p);
			p_sine = sin(p);
			v.push_back({ vec3(sine * p_cosine, sine * p_sine, cosine), vec3(sine * p_cosine, sine * p_sine, cosine), vec3(p / (2 * PI), 1 - t / PI, 1) });
		}
	}
	return v;
}

void update_test_vertex_buffer(const std::vector<vertex>& vertices)
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
		for (uint k = 0; k <= (36 + 1) * 72; k++)
		{
			indices.push_back(k);	//origin
			indices.push_back(k + 72 + 1);
			indices.push_back(k + 1);

			indices.push_back(k + 1);
			indices.push_back(k + 72 + 1);
			indices.push_back(k + 72 + 2);
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

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (test_vertex_array) glDeleteVertexArrays(1, &test_vertex_array);
	test_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!test_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_cube_vertices(uint N)
{
	std::vector<vertex> v;

	for (uint k = 0; k < N / 2; k++)
	{
		float t = PI * 2.0f * k / float(N / 2), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s, 1.0f), vec3(0,0,-1.0f), vec3(0.878431f,1.0f, 1.0f) });
	}
	for (uint k = 0; k < N / 2; k++)
	{
		float t = PI * 2.0f * k / float(N / 2), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s, -1.0f), vec3(0,0,-1.0f), vec3(0.878431f,1.0f, 1.0f) });
	}

	return v;
}

void update_cube_vertex_buffer(const std::vector<vertex>& vertices, uint N, uint k)
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

	if (k == 0) {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (easymode_vertex_array) glDeleteVertexArrays(1, &easymode_vertex_array);
		easymode_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!easymode_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
	else if (k == 1) {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (hellmode_vertex_array) glDeleteVertexArrays(1, &hellmode_vertex_array);
		hellmode_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!hellmode_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
	else {
		// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
		if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
		vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
		if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
	}
}

std::vector<vertex> create_hexcell_vertices2(uint N)
{
	std::vector<vertex> v = { { vec3(0,0,0), vec3(0,1,0), vec3(cell_color_x, cell_color_y, cell_color_z) } }; // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N) + PI * 1.0f * 1 / float(6), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,0,s), vec3(c,1,s), vec3(cell_color_x, cell_color_y, cell_color_z) + 0.3f });
	}
	return v;
}

//	update_desc_hexcell_vertex_buffer
//GLuint vertex_array;
void update_desc_hexcell_vertex_buffer(const std::vector<vertex>& vertices, uint N)
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
		}

		// generation of vertex buffer: use triangle_vertices instead of vertices
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }

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

			indices.push_back(N + 2);	// the origin
			indices.push_back(N + 2 + k + 1);
			indices.push_back(N + 2 + k + 2); //2N+3

			indices.push_back(2 * N + 4);	// the origin
			indices.push_back(2 * N + 4 + k + 1);
			indices.push_back(2 * N + 4 + k + 2); //2N+3

			indices.push_back(k + 1);
			indices.push_back(k + 2);
			indices.push_back(2 * N + 5 + k + 1);

			indices.push_back(2 * N + 5 + k);
			indices.push_back(k + 1);
			indices.push_back(2 * N + 5 + k + 1);

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

			v.push_back(vertices[N + 2]);	// the origin
			v.push_back(vertices[N + 2 + k + 1]);
			v.push_back(vertices[N + 2 + k + 2]);

			v.push_back(vertices[2 * N + 4]);	// the origin
			v.push_back(vertices[2 * N + 4 + k + 1]);
			v.push_back(vertices[2 * N + 4 + k + 2]);

			v.push_back(vertices[k + 1]);
			v.push_back(vertices[k + 2]);
			v.push_back(vertices[2 * N + 5 + k + 1]);

			v.push_back(vertices[2 * N + 5 + k]);
			v.push_back(vertices[k + 1]);
			v.push_back(vertices[2 * N + 5 + k + 1]);

			v.push_back(vertices[N + 2 + k + 1]);
			v.push_back(vertices[N + 2 + k + 2]);
			v.push_back(vertices[2 * N + 5 + k + 1]);

			v.push_back(vertices[2 * N + 5 + k]);
			v.push_back(vertices[N + 2 + k + 1]);
			v.push_back(vertices[2 * N + 5 + k + 1]);
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

std::vector<vertex> create_vert_update_idx_title()
{
	std::vector<vertex> v;

	static GLuint vertex_buffer = 0;   // ID holder for vertex buffer
	static GLuint index_buffer = 0;      // ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)   glDeleteBuffers(1, &vertex_buffer);   vertex_buffer = 0;
	if (index_buffer)   glDeleteBuffers(1, &index_buffer);   index_buffer = 0;

	FILE* file = fopen(title_obj_path, "r");
	std::vector<uint> indices;
	uint cnt = 0;

	while (1) {
		char lineHeader[128];

		// read first line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		// read vertices
		if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex, norm;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			norm = vertex.normalize();

			if (vertex.z < 15)
				v.push_back({ vertex, norm, vec3(0.67f,0.65f,0.878f) });
			else
				v.push_back({ vertex, norm, vec3(0.878f,0.682f,0.333f) });

		}

		// read faces
		else if (strcmp(lineHeader, "f") == 0) {
			uint v1, v2, v3;
			int matches = fscanf(file, "%d %d %d\n", &v1, &v2, &v3);
			indices.push_back(v1 - 1);
			indices.push_back(v2 - 1);
			indices.push_back(v3 - 1);
		}
		cnt++;
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (title_vertex_array) glDeleteVertexArrays(1, &title_vertex_array);
	title_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!title_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return {}; }

	return v;
}

std::vector<vertex> create_vert_update_obj(const char* path, GLuint& vao)
{
	std::vector<vertex> vertices;// = { { vec3(0), vec3(0,0,-1.0f), vec3(0.5f) } }; // origin

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	std::vector<vec3> verts;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;
	std::vector<uint> indices;
	std::vector<char*> mtls;

	bool res = loadOBJ(path, verts, uvs, normals, indices, mtls);
	printf("verts.size(): %d\n", verts.size());
	for (uint i = 0; i < verts.size(); i++) {
		vec3 v = verts.at(i);
		vec3 n = normals.at(i);
		vec2 t = uvs.at(i);
		vertices.push_back({ v,n,vec3(t.x, t.y,0.f) });
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vao) glDeleteVertexArrays(1, &vao);
	vao = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vao) { printf("%s(): failed to create vertex aray\n", __func__); return {}; }

	return vertices;
}

std::vector<vertex> create_vert_update_player()
{
	std::vector<vertex> vertices;// = { { vec3(0), vec3(0,0,-1.0f), vec3(0.5f) } }; // origin

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	std::vector<vec3> verts;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;
	std::vector<uint> indices;
	std::vector<char*> mtls;

	bool res = loadOBJ(player_obj_path, verts, uvs, normals, indices, mtls);
	printf("verts.size(): %d\n", verts.size());
	for (uint i = 0; i < verts.size(); i++) {
		vec3 v = verts.at(i);
		//vec3 v = vec3(verts.at(i).x + 8.f, verts.at(i).y, verts.at(i).z - 25.f);
		vec3 n = normals.at(i);

		vec2 t = uvs.at(i);
		if (v.z > 6.2f)
			vertices.push_back({ v, n, vec3(0.796f,0.909f,0.933f) });
		else if (v.y > 32.f)
			vertices.push_back({ v, n, vec3(1.f,1.f,1.f) });
		else
			vertices.push_back({ v, n, vec3(0.6f,0.427f,0.301f) });
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (player_vertex_array) glDeleteVertexArrays(1, &player_vertex_array);
	player_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!player_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return {}; }

	return vertices;
}

std::vector<vertex> create_vert_update_npc()
{
	std::vector<vertex> vertices;// = { { vec3(0), vec3(0,0,-1.0f), vec3(0.5f) } }; // origin

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	std::vector<vec3> verts;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;
	std::vector<uint> indices;
	std::vector<char*> mtls;

	bool res = loadOBJ(npc_obj_path, verts, uvs, normals, indices, mtls);
	printf("verts.size(): %d\n", verts.size());
	for (uint i = 0; i < verts.size(); i++) {
		vec3 v = verts.at(i);
		vec3 n = normals.at(i);
		vec2 t = uvs.at(i);
		//		vertices.push_back({ v,n,vec3(t.x, t.y,0.f) ;
		if (v.z > 8.5f)
			vertices.push_back({ v, n, vec3(0.796f,0.909f,0.933f) });
		else
			vertices.push_back({ v, n, vec3(0.f,0.f,0.f) });
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (npc_vertex_array) glDeleteVertexArrays(1, &npc_vertex_array);
	npc_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!npc_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return {}; }

	return vertices;
}

std::vector<vertex> create_vert_update_kalbi()
{
	std::vector<vertex> vertices;// = { { vec3(0), vec3(0,0,-1.0f), vec3(0.5f) } }; // origin

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	std::vector<vec3> verts;
	std::vector<vec3> normals;
	std::vector<vec2> uvs;
	std::vector<uint> indices;
	std::vector<char*> mtls;

	bool res = loadOBJ(kalbi_obj_path, verts, uvs, normals, indices, mtls);
	printf("verts.size(): %d\n", verts.size());
	for (uint i = 0; i < verts.size(); i++) {
		vec3 v = verts.at(i);
		vec3 n = normals.at(i);
		vec2 t = uvs.at(i);
		if (v.y < 6.f)
			vertices.push_back({ v, n, vec3(0.6f,0.427f,0.301f) });
		else
			vertices.push_back({ v, n, vec3(1.f,1.f,1.f) });

	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (kalbi_vertex_array) glDeleteVertexArrays(1, &kalbi_vertex_array);
	kalbi_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!kalbi_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return {}; }

	return vertices;
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H)	print_help();
		else if (key == GLFW_KEY_F1 && !b_ready && !b_started && !b_starting_zoom && !b_restart)
		{
			b_description = !b_description;
			if (b_description) {
				cam.eye = vec3(0.0f, -15000.0f, 0.0f);
				cam.at = vec3(0.0f, 0.0f, 1.0f);
				cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			}
			else {
				cam = camera();
			}
		}
		else if (key == GLFW_KEY_1 && !b_started && !b_description && !b_restart && !b_won) {
			level = 1;
			npc = std::move(create_NPCs(level));
			num_npc_left = 9 * level;
			cam.eye = vec3(0, -168, 4);
			cam.at = vec3(0, 0, 0);
			cam.up = vec3(0, 0, 1);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			b_starting_zoom = true;
			t_check = float(glfwGetTime());
			//create engine
			engine = irrklang::createIrrKlangDevice();
			if (!engine) return;
			//add sound source from the sound file
			bgm = engine->addSoundSourceFromFile(easy_bgm_path);

			//play sound file
			engine->play2D(bgm, true);
			bgm->setDefaultVolume(0.6f);
		}
		else if (key == GLFW_KEY_2 && !b_started && !b_description && !b_restart && !b_won) {
			level = 2;
			npc = std::move(create_NPCs(level));
			cam.eye = vec3(0, -168, 4);
			cam.at = vec3(0, 0, 0);
			cam.up = vec3(0, 0, 1);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			b_starting_zoom = true;
			t_check = float(glfwGetTime());
			//create engine
			engine = irrklang::createIrrKlangDevice();
			if (!engine) return;
			//add sound source from the sound file
			bgm = engine->addSoundSourceFromFile(easy_bgm_path);
			//play sound file
			engine->play2D(bgm, true);
			bgm->setDefaultVolume(0.6f);
		}
		else if (key == GLFW_KEY_3 && !b_started && !b_description && !b_restart && !b_won) {
			level = 3;
			npc = std::move(create_NPCs(level));
			cam.eye = vec3(0, -168, 4);
			cam.at = vec3(0, 0, 0);
			cam.up = vec3(0, 0, 1);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			b_starting_zoom = true;
			t_check = float(glfwGetTime());
			//create engine
			engine = irrklang::createIrrKlangDevice();
			if (!engine) return;

			//add sound source from the sound file
			bgm = engine->addSoundSourceFromFile(hell_bgm_path);
			//play sound file
			engine->play2D(bgm, true);
			bgm->setDefaultVolume(0.6f);
		}
		else if (key == GLFW_KEY_R)
		{
			npc.clear();
			b_starting_zoom = false;
			b_started = false;
			b_ready = false;
			b_restart = false;
			b_description = false;
			b_won = false;
			cam.eye = vec3(0, -5000, 0);
			cam.at = vec3(0, 0, 0);
			cam.up = vec3(0, 0, 1);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			npc.clear();
			test.center = vec3(0.f, 0.f, 40.f);
			top = std::move(create_hexgrid_top());
			middle = std::move(create_hexgrid_middle());
			bottom = std::move(create_hexgrid_bottom());
			test.HP = 500.0f;
			engine->stopAllSounds();

		}
		else if (key == GLFW_KEY_A) {
			b_user.left = true;
		}
		else if (key == GLFW_KEY_D) {
			b_user.right = true;
		}
		else if (key == GLFW_KEY_W) {
			b_user.front = true;
		}
		else if (key == GLFW_KEY_S) {
			b_user.back = true;
		}
		else if (key == GLFW_KEY_SPACE && b_started && !b_starting_zoom) {
			if (!b_user.jump) {
				//play sound file
				engine->play2D(jumpsound, false);
				jumpsound->setDefaultVolume(0.6f);
				t_jump = float(glfwGetTime());
				b_user.jump = true;
				b_jump_start = true;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) {
			b_user.left = false;
		}
		else if (key == GLFW_KEY_D) {
			b_user.right = false;
		}
		else if (key == GLFW_KEY_W) {
			b_user.front = false;
		}
		else if (key == GLFW_KEY_S) {
			b_user.back = false;
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) {
			tb.begin(cam.view_matrix, npos);
			if (b_started && !b_starting_zoom) {
				//printf("ok?\n");
				engine->play2D(hitsound, false);
				hitsound->setDefaultVolume(0.6f);
				shoot_kalbi();
			}
		}
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
	//if (!tb.is_tracking() && !tb.is_zooming() && !tb.is_panning()) return;
	if (!b_started)	return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	if (tb.is_tracking())cam.view_matrix = tb.update(npos);
	else if (tb.is_zooming())cam.view_matrix = tb.update_zooming(npos);
	else if (tb.is_panning())cam.view_matrix = tb.update_panning(npos);
	else {
		// set view by mouse
		POINT mousePos;
		int middleX = GetSystemMetrics(SM_CXSCREEN) >> 1;
		int middleY = GetSystemMetrics(SM_CYSCREEN) >> 1;
		float angleY = 0.0f;
		//float angleZ = 0.0f;                             
		static float currentRotX = 0.0f;

		GetCursorPos(&mousePos);
		ShowCursor(false);

		//this->count--;

		if ((mousePos.x == middleX))// && (mousePos.y == middleY))
			return;

		SetCursorPos(middleX, middleY);

		angleY = (float)((middleX - mousePos.x)) / ANGLE_Y;
		user_forward += angleY;	// + oder - ?
		cam.view_matrix *= mat4::rotate(vec3(0, 0, 1), -angleY);

	}
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
GLuint create_cubemap(std::vector<const char*> path)
{
	GLuint texture; glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	image* img = cg_load_image(path[0]); if (!img) return -1;
	int w = img->width, h = img->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	if (img) delete img; // release image

	image* img1 = cg_load_image(path[1]); if (!img1) return -1;
	int w1 = img1->width, h1 = img1->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, w1, h1, 0, GL_RGB, GL_UNSIGNED_BYTE, img1->ptr);
	if (img1) delete img1; // release image

	image* img2 = cg_load_image(path[2]); if (!img2) return -1;
	int w2 = img2->width, h2 = img2->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, w2, h2, 0, GL_RGB, GL_UNSIGNED_BYTE, img2->ptr);
	if (img2) delete img2; // release image

	image* img3 = cg_load_image(path[3]); if (!img3) return -1;
	int w3 = img3->width, h3 = img3->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, w3, h3, 0, GL_RGB, GL_UNSIGNED_BYTE, img3->ptr);
	if (img3) delete img3; // release image

	image* img4 = cg_load_image(path[4]); if (!img4) return -1;
	int w4 = img4->width, h4 = img4->height;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, w4, h4, 0, GL_RGB, GL_UNSIGNED_BYTE, img4->ptr);
	if (img4) delete img4; // release image

	image* img5 = cg_load_image(path[5]); if (!img5) return -1;
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
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(0.0f, 0.7f, 0.7f, 1.0f);	// set clear color
	//glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);										// turn on backface culling
	glEnable(GL_DEPTH_TEST);									// turn on depth tests
	glEnable(GL_TEXTURE_CUBE_MAP);								// enable texturing
	glEnable(GL_TEXTURE_2D);									// enable texturing
	glActiveTexture(GL_TEXTURE0);								// notify GL the current texture slot is 0

	cell_color_x = 0.878431f;
	cell_color_y = 1.0f;
	cell_color_z = 1.0f;
	unit_top_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_top_hexcell_vertices, 6, 0);

	cell_color_x = 0.980392f;
	cell_color_y = 0.980392f;
	cell_color_z = 0.823529f;
	unit_middle_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_middle_hexcell_vertices, 6, 1);


	cell_color_x = 1.0f;
	cell_color_y = 0.713725f;
	cell_color_z = 0.756863f;
	unit_bottom_hexcell_vertices = std::move(create_hexcell_vertices(6));
	update_hexcell_vertex_buffer(unit_bottom_hexcell_vertices, 6, 2);

	unit_easymode_vertices = std::move(create_cube_vertices(8));
	update_cube_vertex_buffer(unit_easymode_vertices, 8, 0);

	std::vector<const char*> path;
	path = { "../bin/textures/background_front.jpg",
			"../bin/textures/background_left.jpg",
			"../bin/textures/background_back.jpg",
			"../bin/textures/background_right.jpg",
			"../bin/textures/background_down.jpg",
			"../bin/textures/background_up.jpg",
	};
	EASYMODE = create_cubemap(path);			if (EASYMODE == -1) return false;


	unit_hellmode_vertices = std::move(create_cube_vertices(8));
	update_cube_vertex_buffer(unit_hellmode_vertices, 8, 1);
	path = { "../bin/textures/hell_front.jpg",
			"../bin/textures/hell_right.jpg",
			"../bin/textures/hell_back.jpg",
			"../bin/textures/hell_left.jpg",
			"../bin/textures/hell_up.jpg",
			"../bin/textures/hell_down.jpg"
	};
	HELLMODE = create_cubemap(path);			if (HELLMODE == -1) return false;


	test_vertices = std::move(create_test_vertices());
	update_test_vertex_buffer(test_vertices);

	title_vertices = std::move(create_vert_update_idx_title());
	//title_texture = create_texture(title_obj_path, true);		if (title_texture == -1) return false;

	description1_texture = create_texture(description1_texture_path, true);		if (description1_texture == -1) return false;
	description2_texture = create_texture(description2_texture_path, true);		if (description2_texture == -1) return false;
	description3_texture = create_texture(description3_texture_path, true);		if (description3_texture == -1) return false;

	player_vertices = std::move(create_vert_update_player());
	npc_vertices = std::move(create_vert_update_npc());
	kalbi_vertices = std::move(create_vert_update_kalbi());
	//create engine
	engine = irrklang::createIrrKlangDevice();
	if (!engine) return false;;
	jumpsound = engine->addSoundSourceFromFile(jump_bgm_path);
	readysound = engine->addSoundSourceFromFile(ready_bgm_path);
	gosound = engine->addSoundSourceFromFile(go_bgm_path);
	hitsound = engine->addSoundSourceFromFile(hit_bgm_path);

	unit_desc_hexcell_vertices = std::move(create_hexcell_vertices2(6));
	update_desc_hexcell_vertex_buffer(unit_desc_hexcell_vertices, 6);


	//text
	if (!init_text()) return false;

	return true;

}

void user_finalize()
{
}

int main(int argc, char* argv[])
{
	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!(program2 = cg_create_program(vert_shader_path2, frag_shader_path2))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movement


	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
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

