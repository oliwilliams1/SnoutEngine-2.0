#version 330 core

out vec4 FragColor;

in vec3 fragNormal;
in float height;

vec3 camDirection = vec3(-1.0, -0.6, -1.0);

// Function for linear interpolation
vec4 linearInterpolate(vec4 y0, vec4 y1, float mu) {
    return mix(y0, y1, mu);
}

vec4 getTerrainColor(float value) {
    // Define key colors
    vec4 color1 = vec4(0.0, 0.0, 1.0, 1.0); // dark blue
    vec4 color2 = vec4(0.0, 0.5, 1.0, 1.0); // light blue
    vec4 color3 = vec4(0.5, 1.0, 0.5, 1.0); // green
    vec4 color4 = vec4(1.0, 1.0, 0.0, 1.0); // brown
    vec4 color5 = vec4(1.0, 1.0, 1.0, 1.0); // white
    vec4 color6 = vec4(1.0, 1.0, 1.0, 1.0); // white again

    float mu;

    if (value < -1.0) {
        return color1; // Deep water
    } else if (value < 0.0) {
        mu = (value + 1.0) / 1.0; // Normalize to [0, 1]
        return linearInterpolate(color1, color2, mu);
    } else if (value < 0.5) {
        mu = value / 0.5; // Normalize to [0, 1]
        return linearInterpolate(color2, color3, mu);
    } else if (value < 1.0) {
        mu = (value - 0.5) / 0.5; // Normalize to [0, 1]
        return linearInterpolate(color3, color4, mu);
    } else if (value < 1.3) {
        mu = (value - 1.0) / 0.3; // Normalize to [0, 1]
        return linearInterpolate(color4, color5, mu);
    } else {
        return color6; // Snow
    }
}

void main()
{
    float f = dot(-camDirection, normalize(fragNormal)); // Normalize normal for accurate lighting
    vec4 terrainColor = getTerrainColor(height); // Get color as vec4
    vec3 colour = terrainColor.rgb; // Extract RGB from vec4
    colour *= f; // Apply lighting factor
    FragColor = vec4(colour, 1.0); // Set final fragment color
}