#version 330 core

out vec4 FragColor;

in vec3 color;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(1.2, 1.0, -2.0);

// Function to convert RGB to HSV
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// Function to convert HSV to RGB
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Ambient
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate shadow color by adjusting hue, saturation, and brightness
    vec3 hsv = rgb2hsv(color);
    hsv.x = mod(hsv.x - 0.075, 1.0); // Increase hue (0.1 is the amount to shift, adjust as needed)
    hsv.y = min(hsv.y + 0.038, 1.0); // Increase saturation (0.3 is the amount to increase, adjust as needed)
    hsv.z = hsv.z * 0.6; // Decrease brightness (0.5 is the factor to decrease, adjust as needed)
    vec3 shadowColor = hsv2rgb(hsv);

    // Combine results with shadow consideration
    vec3 result = ambient + diffuse;
    result = mix(shadowColor, result, diff); // Interpolate between shadow color and lit color based on diffuse factor

    FragColor = vec4(result * color, 1.0);
}
