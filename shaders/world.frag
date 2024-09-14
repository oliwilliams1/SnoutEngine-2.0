#version 330 core

out vec4 FragColor;

in vec3 fragNormal;
in float height;

vec3 camDirection = vec3(-0.5, 1.0, -0.5);

// Function for linear interpolation
vec4 linearInterpolate(vec4 y0, vec4 y1, float mu) {
    return mix(y0, y1, mu);
}

void main()
{
    float f = pow(dot(camDirection, normalize(fragNormal)), 0.5); // Normalize normal for accurate lighting
    vec3 colour = vec3(0.75, 0.75, 0.75);
    colour *= f; // Apply lighting factor
    FragColor = vec4(colour, 1.0); // Set final fragment color
}