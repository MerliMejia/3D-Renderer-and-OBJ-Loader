#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <stdio.h>

GLuint genShader(const char *fileName, GLenum shaderType);

GLuint genShaderProgram(GLuint *shaders, int numShaders);

#endif // SHADERS_H