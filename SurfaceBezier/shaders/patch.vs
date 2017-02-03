#version 410 core

in vec4 position;
uniform mat4 model_matrix;

void main(void)
{
    gl_Position = model_matrix * position;
}
