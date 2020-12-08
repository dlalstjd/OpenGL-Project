layout (location=0) in vec3 position;
layout (location=2) in vec3 texcoord;

out vec3 tc;

uniform mat4 text_matrix;

void main()
{
    gl_Position = text_matrix * vec4(position,1.0);
    tc = texcoord;
}  
