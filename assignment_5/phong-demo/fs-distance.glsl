#version 400
out vec4 frag_color;
in vec3 vert_pos, vert_norm;

// light position
vec3 light_pos = vec3(0.0, 1.0, 1.0);

void main() {
    // Distance between fragment and light source
    float distance = length(light_pos - vert_pos);

    // Inverse square falloff
    float brightness = 1.0 / (distance * distance);

    // Set all color channels to brightness (white light, closer = brighter)
    frag_color = vec4(brightness, brightness, brightness, 1.0);
}