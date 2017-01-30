#version 150

attribute vec3 a_position;
attribute vec4 a_color;
uniform mat4 projview_matrix;
uniform mat4 model_matrix;

out vec4 color;

void main()
{
	color = a_color;
    gl_Position = projview_matrix * model_matrix * vec4(a_position, 1.0f) ; 
}