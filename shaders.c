#include <GL/glew.h>
#include <stdio.h>
#include "utils.h"

GLuint genShader(const char *fileName, GLenum shaderType)
{
    // Read shader source from file
    char *shaderSource = readShaderSource(fileName);
    if (!shaderSource)
    {
        return 0;
    }

    // Compile the shader
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check for shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        return 0;
    }

    return shader;
}

GLuint genShaderProgram(GLuint *shaders, int numShaders)
{
    // Link the shaders into a shader program
    GLuint shaderProgram = glCreateProgram();
    for (int i = 0; i < numShaders; i++)
    {
        glAttachShader(shaderProgram, shaders[i]);
    }
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        return 0;
    }

    return shaderProgram;
}