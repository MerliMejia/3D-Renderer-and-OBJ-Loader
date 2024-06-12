#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 normal;

out vec3 color;
out vec3 Normal;
out vec3 FragPos;

uniform float time;

mat4 model = mat4(1.0);

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
    mat4 rotation = rotationMatrix(vec3(0.0, 1.0, 0.0), time);
    vec4 worldPosition = model * rotation * vec4(position, 5.0);
    gl_Position = worldPosition;

    color = vertexColor;
    Normal = mat3(transpose(inverse(model * rotation))) * normal; // Transforming normal
    FragPos = vec3(worldPosition); // World space position
}
