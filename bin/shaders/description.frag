#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec3 tc;
in vec3 pos;
in vec3 norm;

// the only output variable
out vec4 Color;

// texture sampler
uniform sampler2D	description1_texture;
uniform sampler2D	description2_texture;
uniform sampler2D	description3_texture;

uniform int		mode2;

void main()
{
	if(mode2==5)			Color = texture( description1_texture, norm.xz);
	else if(mode2==6)		Color = texture( description2_texture, norm.xz );
	else if(mode2==7)		Color = texture( description3_texture, norm.xz );
}
