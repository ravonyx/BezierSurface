#pragma once

#include <GL/glew.h>

GLuint CreateTexture(const char* name);
void _check_gl_error(const char *file, int line);

#define check_gl_error() _check_gl_error(__FILE__,__LINE__)