#version 150

attribute vec3 a_position;
attribute vec4 a_color;
uniform mat4 VP;
uniform mat4 M;

out vec4 color;

void main()
{
	color = a_color;
    gl_Position = VP * M * vec4(a_position, 1.0f) ; 
}