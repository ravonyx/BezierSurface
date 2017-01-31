#version 410 core

in vec4 position;
uniform mat4 model_matrix;

out vec4 positionOut;

void main(void)
{
	positionOut = model_matrix * position;
    gl_Position = model_matrix * position;
}
