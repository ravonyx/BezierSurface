#version 150

layout(points) in;
layout(line_strip, max_vertices = 88) out;

uniform mat4 projview_matrix;


void main()
{
	float size = 2;
	float spacing = size / 10;
	for(int i = -10; i <= 10; i++)
	{
		gl_Position = projview_matrix*(gl_in[0].gl_Position + vec4(0, 0, spacing * i, 0.0) + vec4(size, 0, 0, 0.0));
		EmitVertex();
		gl_Position = projview_matrix*(gl_in[0].gl_Position + vec4(0, 0,spacing * i, 0.0) + vec4(-size, 0, 0, 0.0));
		EmitVertex();
		EndPrimitive();

		gl_Position = projview_matrix*(gl_in[0].gl_Position + vec4(spacing * i, 0, 0, 0.0) + vec4(0, 0, size, 0.0));
		EmitVertex();
		gl_Position = projview_matrix*(gl_in[0].gl_Position + vec4(spacing * i, 0, 0, 0.0) + vec4(0, 0, -size, 0.0));
		EmitVertex();
		EndPrimitive();
	}

}