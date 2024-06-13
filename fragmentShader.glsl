#version 330 core

out vec4 FragColor;

in vec3 color;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(1.2, 1.0, -2.0);

// For some objects, 2.2 is a better gamma value
float gamma = 1.8;

void main() {
    // Ambient
    float ambientStrength = 0.7;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    float specularExponent = 32.0;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    vec3 specular = specularStrength * spec * lightColor;

    float kd_corrected_r = pow(color.r, 1.0 / gamma);
    float kd_corrected_g = pow(color.g, 1.0 / gamma);
    float kd_corrected_b = pow(color.b, 1.0 / gamma);

    // Final color
    vec3 result = (ambient + diffuse + specular) * vec3(kd_corrected_r, kd_corrected_g, kd_corrected_b);
    FragColor = vec4(result, 1.0);
}
