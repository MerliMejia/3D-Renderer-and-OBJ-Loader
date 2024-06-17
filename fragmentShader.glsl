#version 330 core

out vec4 FragColor;

in vec3 color;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(1.2, 1.0, -2.0);

uniform float hueAdjust = 0.05;
uniform float saturationAdjust = 1.8;
uniform float brightnessAdjust = 0.2;

uniform float hHueAdjust = 0.05;
uniform float hSaturationAdjust = 1.8;
uniform float hBrightnessAdjust = 0.2;

// For some objects, 2.2 is a better gamma value
uniform float gamma = 2.2;

uniform float ambientStrength = 0.1;

uniform float specularStrength = 0.8;
uniform float specularExponent = 32.0;

vec3 rgb2hsb(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsb2rgb(vec3 c) {
    vec3 p = abs(fract(c.xxx + vec3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0);
    return c.z * mix(vec3(1.0), clamp(p - vec3(1.0), 0.0, 1.0), c.y);
}

void main() {
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    vec3 specular = specularStrength * spec * lightColor;

    float kd_corrected_r = pow(color.r, 1.0 / gamma);
    float kd_corrected_g = pow(color.g, 1.0 / gamma);
    float kd_corrected_b = pow(color.b, 1.0 / gamma);

    vec3 kd_corrected = vec3(kd_corrected_r, kd_corrected_g, kd_corrected_b);

    vec3 shadowColor = rgb2hsb(kd_corrected);
    shadowColor.r -= hueAdjust;
    shadowColor.g *= saturationAdjust;
    shadowColor.b -= brightnessAdjust;

    shadowColor = hsb2rgb(shadowColor);

    vec3 highligthColor = rgb2hsb(kd_corrected);
    highligthColor.r += hHueAdjust;
    highligthColor.g *= hSaturationAdjust;
    highligthColor.b += hBrightnessAdjust;

    highligthColor = hsb2rgb(highligthColor);

    vec3 result = (ambient + diffuse + shadowColor + specular + highligthColor) * kd_corrected;
    FragColor = vec4(result, 1.0);
}
