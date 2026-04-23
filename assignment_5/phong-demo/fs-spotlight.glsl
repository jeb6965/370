#version 400
out vec4 frag_color;
in vec3 vert_pos, vert_norm;

// light properties
vec3 l_a = vec3(0.2, 0.2, 0.2);
vec3 l_d = vec3(0.7, 0.7, 0.7);
vec3 l_s = vec3(1.0, 1.0, 1.0);
// material properties
vec3 m_a = vec3(1.0, 1.0, 1.0);
vec3 m_d = vec3(0.0, 1.0, 0.0);
vec3 m_s = vec3(1.0, 1.0, 1.0);
float specular_component = 10.0;

// Light position — offset to one side so only part of model is lit
vec3 light_pos = vec3(-0.9, 0.5, 1.0);
// Spotlight direction — pointing toward the model's left side
vec3 spot_dir = normalize(vec3(0.9, -0.5, -1.0));
// Spotlight angle — half-angle of the cone in degrees
float spot_cutoff = cos(radians(10.0));

void main() {
    vec3 N = normalize(vert_norm);
    vec3 L = normalize(light_pos - vert_pos);
    vec3 V = normalize(-vert_pos);

    // Distance attenuation: 1 / distance^2
    float dist = length(light_pos - vert_pos);
    float attenuation = 1.0 / (dist * dist);

    // Ambient always applied
    vec3 Ia = l_a * m_a;

    // Check if fragment is inside the spotlight cone
    // dot of L flipped (fragment->light) against spot direction gives cosine of angle
    float spot_angle = dot(-L, spot_dir);

    if (spot_angle > spot_cutoff) {
        // Inside cone: full Phong + attenuation
        float diff = max(dot(N, L), 0.0);
        vec3 Id = l_d * m_d * diff * attenuation;

        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), specular_component);
        vec3 Is = l_s * m_s * spec * attenuation;

        frag_color = vec4(Ia + Id + Is, 1.0);
    } else {
        // Outside cone: ambient only
        frag_color = vec4(Ia, 1.0);
    }
}