#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec3 pos;
in vec3 tc;

// the only output variable
out vec4 fragColor;

uniform mat4	model_matrix;
uniform mat4	view_matrix;
uniform float	shininess;
uniform float	t;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties
uniform vec4	solid_color;
uniform vec4	color;


// texture sampler
uniform samplerCube EASYMODE;
uniform samplerCube HELLMODE;
uniform sampler2D	description1_texture;
uniform sampler2D	description2_texture;
uniform sampler2D	description3_texture;

uniform int		mode;
uniform int		title;


vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{
	// light position in the eye space
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	if (mode==0)		fragColor = texture( EASYMODE, pos );
	else if(mode==1)	fragColor = vec4 (tc.xyz ,1);
	else if(mode==2)	fragColor = solid_color;
	else if(mode==4)	fragColor = texture( HELLMODE, pos );
}
