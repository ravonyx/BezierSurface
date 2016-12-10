#version 120

uniform vec4 attr_color;

void main()
{
	vec4 color;
	color.x = 0.0f;
	color.y = 0.0f;
	color.z = 255.0f;
	color.w = 1.0f;
    gl_FragColor = color;
}