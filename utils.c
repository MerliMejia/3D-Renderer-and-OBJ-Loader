#include <stdio.h>
#include <stdlib.h>

char *readShaderSource(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open shader file: %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    if (!buffer)
    {
        fprintf(stderr, "Failed to allocate memory for shader source\n");
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}