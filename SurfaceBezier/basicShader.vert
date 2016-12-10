#version 120

attribute vec3 a_position;
uniform mat4 VP;
uniform mat4 M;

void main()
{
    gl_Position = VP * M * vec4(a_position, 1.0f) ; 
}