#include <GL/glew.h>
#include "glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shaders.h"
#include <math.h>

typedef struct
{
    float x, y, z;
} Vertex;

typedef struct
{
    float u, v;
} TexCoord;

typedef struct
{
    float x, y, z;
} Normal;

typedef struct
{
    int vertexIndex[3];
    int texCoordIndex[3];
    int normalIndex[3];
    char materialName[50];
} Face;

typedef struct
{
    char name[50];
    float Ka[3];     // Ambient color
    float Kd[3];     // Diffuse color
    float Ks[3];     // Specular color
    float Ns;        // Specular exponent
    float Ni;        // Optical density (refraction index)
    float d;         // Dissolve (transparency)
    int illum;       // Illumination model
    char map_Kd[50]; // Diffuse texture map
} Material;

// #define MAX_VERTICES 100000

// Vertex vertices[MAX_VERTICES];
// int vertex_count = 0;

// #define MAX_TEXCOORDS 100000

// TexCoord texCoords[MAX_TEXCOORDS];
// int texCoord_count = 0;

// #define MAX_NORMALS 100000

// Normal normals[MAX_NORMALS];
// int normal_count = 0;

// #define MAX_FACES 1000000

// Face faces[MAX_FACES];
// int face_count = 0;

#define MAX_MATERIALS 100

Material materials[MAX_MATERIALS];
int material_count = 0;

void read_obj_file(const char *filename, Vertex **vertices, int *vertex_count, int *vertex_capacity, TexCoord **texCoords, int *textCoord_count, int *texCoord_capacity, Normal **normals, int *normal_count, int *normal_capacity, Face **faces, int *face_count, int *face_capacity)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[128];
    char current_material[50] = "";

    // Initialize vertex count and capacity
    *vertex_count = 0;
    *vertex_capacity = 10; // Initial capacity, can be adjusted as needed
    *vertices = malloc(*vertex_capacity * sizeof(Vertex));

    // Initialize texture coordinate count and capacity
    *textCoord_count = 0;
    *texCoord_capacity = 10; // Initial capacity, can be adjusted as needed
    *texCoords = malloc(*texCoord_capacity * sizeof(TexCoord));

    // Initialize normal count and capacity
    *normal_count = 0;
    *normal_capacity = 10; // Initial capacity, can be adjusted as needed
    *normals = malloc(*normal_capacity * sizeof(Normal));

    // Initialize face count and capacity
    *face_count = 0;
    *face_capacity = 10; // Initial capacity, can be adjusted as needed
    *faces = malloc(*face_capacity * sizeof(Face));

    if (!*vertices)
    {
        perror("Failed to allocate memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            Vertex vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);

            if (*vertex_count >= *vertex_capacity)
            {
                *vertex_capacity *= 2; // Double the capacity
                *vertices = realloc(*vertices, *vertex_capacity * sizeof(Vertex));
                if (!*vertices)
                {
                    perror("Failed to reallocate memory");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }
            }

            (*vertices)[(*vertex_count)++] = vertex;
        }
        else if (strncmp(line, "vt ", 3) == 0)
        {
            TexCoord texCoord;
            sscanf(line, "vt %f %f", &texCoord.u, &texCoord.v);

            if (*textCoord_count >= *texCoord_capacity)
            {
                *texCoord_capacity *= 2; // Double the capacity
                *texCoords = realloc(*texCoords, *texCoord_capacity * sizeof(TexCoord));
                if (!*texCoords)
                {
                    perror("Failed to reallocate memory");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }
            }

            (*texCoords)[(*textCoord_count)++] = texCoord;
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            Normal normal;
            sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);

            if (*normal_count >= *normal_capacity)
            {
                *normal_capacity *= 2; // Double the capacity
                *normals = realloc(*normals, *normal_capacity * sizeof(Normal));
                if (!*normals)
                {
                    perror("Failed to reallocate memory");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }
            }

            (*normals)[(*normal_count)++] = normal;
        }
        else if (strncmp(line, "usemtl ", 7) == 0)
        {
            sscanf(line, "usemtl %s", current_material);
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            Face face;
            int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                                 &face.vertexIndex[0], &face.texCoordIndex[0], &face.normalIndex[0],
                                 &face.vertexIndex[1], &face.texCoordIndex[1], &face.normalIndex[1],
                                 &face.vertexIndex[2], &face.texCoordIndex[2], &face.normalIndex[2]);

            if (matches == 9)
            {
                strcpy(face.materialName, current_material);

                if (*face_count >= *face_capacity)
                {
                    *face_capacity *= 2; // Double the capacity
                    *faces = realloc(*faces, *face_capacity * sizeof(Face));
                    if (!*faces)
                    {
                        perror("Failed to reallocate memory");
                        fclose(file);
                        exit(EXIT_FAILURE);
                    }
                }

                (*faces)[(*face_count)++] = face;
            }
            else
            {
                fprintf(stderr, "Error: Expected 9 values for face, got %d\n", matches);
            }
        }
    }

    fclose(file);
}

void read_mtl_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[128];
    Material *current_material = NULL;

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "newmtl ", 7) == 0)
        {
            current_material = &materials[material_count++];
            sscanf(line, "newmtl %s", current_material->name);
        }
        else if (current_material)
        {
            if (strncmp(line, "Ka ", 3) == 0)
            {
                sscanf(line, "Ka %f %f %f", &current_material->Ka[0], &current_material->Ka[1], &current_material->Ka[2]);
            }
            else if (strncmp(line, "Kd ", 3) == 0)
            {
                sscanf(line, "Kd %f %f %f", &current_material->Kd[0], &current_material->Kd[1], &current_material->Kd[2]);
            }
            else if (strncmp(line, "Ks ", 3) == 0)
            {
                sscanf(line, "Ks %f %f %f", &current_material->Ks[0], &current_material->Ks[1], &current_material->Ks[2]);
            }
            else if (strncmp(line, "Ns ", 3) == 0)
            {
                sscanf(line, "Ns %f", &current_material->Ns);
            }
            else if (strncmp(line, "Ni ", 3) == 0)
            {
                sscanf(line, "Ni %f", &current_material->Ni);
            }
            else if (strncmp(line, "d ", 2) == 0)
            {
                sscanf(line, "d %f", &current_material->d);
            }
            else if (strncmp(line, "illum ", 6) == 0)
            {
                sscanf(line, "illum %d", &current_material->illum);
            }
            else if (strncmp(line, "map_Kd ", 7) == 0)
            {
                sscanf(line, "map_Kd %s", current_material->map_Kd);
            }
        }
    }

    fclose(file);
}

int main()
{
    printf("\nReading OBJ file...\n\n");

    Vertex *vertices = NULL;
    int vertex_count = 0;
    int vertex_capacity = 0;

    TexCoord *texCoords = NULL;
    int texCoord_count = 0;
    int texCoord_capacity = 0;

    Normal *normals = NULL;
    int normal_count = 0;
    int normal_capacity = 0;

    Face *faces = NULL;
    int face_count = 0;
    int face_capacity = 0;

    read_mtl_file("esposito.mtl");
    read_obj_file("esposito.obj", &vertices, &vertex_count, &vertex_capacity, &texCoords, &texCoord_count, &texCoord_capacity, &normals, &normal_count, &normal_capacity, &faces, &face_count, &face_capacity);

    // printf("Materials:\n");
    // for (int i = 0; i < material_count; i++)
    // {
    //     printf("Material %d: %s\n", i + 1, materials[i].name);
    //     printf("  Ka: %f %f %f\n", materials[i].Ka[0], materials[i].Ka[1], materials[i].Ka[2]);
    //     printf("  Kd: %f %f %f\n", materials[i].Kd[0], materials[i].Kd[1], materials[i].Kd[2]);
    //     printf("  Ks: %f %f %f\n", materials[i].Ks[0], materials[i].Ks[1], materials[i].Ks[2]);
    //     printf("  Ns: %f\n", materials[i].Ns);
    //     printf("  Ni: %f\n", materials[i].Ni);
    //     printf("  d: %f\n", materials[i].d);
    //     printf("  illum: %d\n", materials[i].illum);
    //     printf("  map_Kd: %s\n", materials[i].map_Kd);
    // }

    // for (int i = 0; i < vertex_count; i++)
    // {
    //     printf("Vertex %d: x=%f, y=%f, z=%f\n", i + 1, vertices[i].x, vertices[i].y, vertices[i].z);
    // }

    // printf("Texture Coordinates:\n");
    // for (int i = 0; i < texCoord_count; i++)
    // {
    //     printf("TexCoord %d: u=%f, v=%f\n", i + 1, texCoords[i].u, texCoords[i].v);
    // }

    // printf("Normals:\n");
    // for (int i = 0; i < normal_count; i++)
    // {
    //     printf("Normal %d: x=%f, y=%f, z=%f\n", i + 1, normals[i].x, normals[i].y, normals[i].z);
    // }

    // printf("Faces:\n");
    // for (int i = 0; i < face_count; i++)
    // {
    //     printf("Face %d: ", i + 1);
    //     for (int j = 0; j < 3; j++)
    //     {
    //         printf("%d/%d/%d ", faces[i].vertexIndex[j], faces[i].texCoordIndex[j], faces[i].normalIndex[j]);
    //     }
    //     printf("\n");
    // }

    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Set GLFW window hints for OpenGL version (adjust to your needs)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Ensure GLEW uses modern techniques for managing OpenGL functionality
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(err));
        return -1;
    }

    // Query and print the OpenGL version
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    printf("OpenGL Version: %s\n", version);
    printf("Renderer: %s\n", renderer);
    printf("Vendor: %s\n", vendor);
    printf("GLSL Version: %s\n", shadingLanguageVersion);
    printf("Max Attributes: %d\n", maxVertexAttribs);

    GLint maxVertexUniforms, maxFragmentUniforms, maxGeometryUniforms;

    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniforms);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniforms);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &maxGeometryUniforms);

    printf("Max Vertex Uniforms: %d\n", maxVertexUniforms);
    printf("Max Fragment Uniforms: %d\n", maxFragmentUniforms);
    printf("Max Geometry Uniforms: %d\n", maxGeometryUniforms);

    // Print how many vertices, texCoords, normals, and faces were read
    printf("\nObj Amounts of memory allocated:\n\n");
    printf("Vertices: %d\n", vertex_capacity);
    printf("TexCoords: %d\n", texCoord_capacity);
    printf("Normals: %d\n", normal_capacity);
    printf("Faces: %d\n", face_capacity);

    printf("\nPassing data to GPU...\n");

    GLfloat verticesToGPU[face_count * 9];

    for (int i = 0; i < vertex_count; i++)
    {
        verticesToGPU[i * 9] = vertices[i].x;
        verticesToGPU[i * 9 + 1] = vertices[i].y;
        verticesToGPU[i * 9 + 2] = vertices[i].z;

        Face *face = NULL;

        for (int j = 0; j < face_count; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (faces[j].vertexIndex[k] == i + 1)
                {
                    face = &faces[j];
                    break;
                }
            }
        }

        if (face)
        {
            // printf("Face %d: %s\n", i + 1, face->materialName);
            for (int j = 0; j < material_count; j++)
            {
                if (strcmp(materials[j].name, face->materialName) == 0)
                {
                    verticesToGPU[i * 9 + 3] = materials[j].Kd[0];
                    verticesToGPU[i * 9 + 4] = materials[j].Kd[1];
                    verticesToGPU[i * 9 + 5] = materials[j].Kd[2];
                }
            }

            verticesToGPU[i * 9 + 6] = normals[face->normalIndex[0] - 1].x;
            verticesToGPU[i * 9 + 7] = normals[face->normalIndex[1] - 1].y;
            verticesToGPU[i * 9 + 8] = normals[face->normalIndex[2] - 1].z;
        }
    }

    GLint indices[face_count * 3];

    for (int i = 0; i < face_count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            indices[i * 3 + j] = faces[i].vertexIndex[j] - 1;
        }
    }

    GLint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesToGPU), verticesToGPU, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    GLuint shaders[2];
    shaders[0] = genShader("vertexShader.glsl", GL_VERTEX_SHADER);
    shaders[1] = genShader("fragmentShader.glsl", GL_FRAGMENT_SHADER);

    GLuint shaderProgram = genShaderProgram(shaders, 2);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    printf("\nFreeing memory...\n");

    free(vertices);
    free(texCoords);
    free(normals);
    free(faces);

    printf("\nRendering...\n");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {

        // Set the clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(shaderProgram);

        // Uniform for time
        GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
        glUniform1f(timeLocation, (GLfloat)glfwGetTime());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, face_count * 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("\nExiting...\n");

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(shaderProgram);

    for (int i = 0; i < 2; i++)
    {
        glDeleteShader(shaders[i]);
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}