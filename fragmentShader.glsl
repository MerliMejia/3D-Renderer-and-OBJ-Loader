#version 330 core

out vec4 FragColor;

in vec3 color;
in vec3 Normal;
in vec3 FragPos;

vec3 lightColor = vec3(1.0, 1.0, 1.0);
vec3 lightPos = vec3(1.2, 1.0, 2.0);

void main() {
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}
