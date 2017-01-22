#version 120

in vec4 color;

void main()
{
	vec4 colorTst;
	colorTst.x = 0.0f;
	colorTst.y = 0.0f;
	colorTst.z = 255.0f;
	colorTst.w = 1.0f;
    gl_FragColor = color;
}