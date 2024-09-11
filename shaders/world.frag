#version 330 core

in float height;
out vec4 fragColor;

void main()
{
    vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 darkBlue = vec4(0.12, 0.0, 0.5, 1.0);
    vec4 brown = vec4(0.2, 0.7, 0.2, 1.0);
    vec4 green = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

    if (height < -1.0) {
        fragColor = blue;
    } else if (height < 0.0) {
        float t = (height + 1.0) / 1.0;
        fragColor = mix(blue, darkBlue, t);
    } else if (height < -0.7) {
        float t = height / 0.3;
        fragColor = mix(darkBlue, brown, t);
    } else if (height < 0.2) {
        float t = (height - 0.3) / (1.0 - 0.3);
        fragColor = mix(brown, green, t);
    } else {
        float t = (height - 1.0) / (1.0 - 1.0);
        fragColor = mix(green, white, t);
    }
}