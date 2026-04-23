#version 400
out vec4 frag_color;
in vec3 vert_pos, vert_norm;

// light properties
vec3 l_a = vec3(0.4, 0.4, 0.4);
vec3 l_d = vec3(0.7, 0.7, 0.7);
vec3 l_s = vec3(1.0, 1.0, 1.0);
// material properties
vec3 m_a = vec3(1.0, 1.0, 1.0);
vec3 m_d = vec3(0.0, 1.0, 0.0);
vec3 m_s = vec3(1.0, 1.0, 1.0);
float specular_component = 10.0;

vec3 light_pos = vec3(0.0, 0.2, 1.0);

void main() {
    vec3 N = normalize(vert_norm);
    vec3 L = normalize(light_pos - vert_pos);
    //vec3 V = normalize(-vert_pos);
    vec3 V = normalize(vec3(0.0, 0.0, 1.0));  // camera looks along +Z

    // Angle between surface normal and light direction
    float diff = max(dot(N, L), 0.0);

    // Thresholding: map diffuse into four discrete bands
    vec3 toon_color;
    if (diff > 0.85)      toon_color = vec3(0.2, 0.0, 0.0);  // dark red
    else if (diff > 0.5)  toon_color = vec3(0.0, 0.7, 0.3);    // teal green
    else if (diff > 0.2)  toon_color = vec3(1.0, 0.9, 0.4);  // bright gold
    else                  toon_color = vec3(0.3, 0.0, 0.6);  // dark purple
    // Replace diffuse component with toon-quantized value
    vec3 Ia = l_a * m_a;
    vec3 Id = l_d * m_d * toon_color;

    // Specular — also quantized for clean highlight
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), specular_component);
    float toon_spec = spec > 0.8 ? 0.6 : 0.0;  // was 0.5, and cap at 0.6 not 1.0
    vec3 Is = l_s * m_s * toon_spec;

    // Edge detection: angle between normal and view direction
    float edge = dot(N, V);

    // If normal is near perpendicular to view, it's an edge — draw dark
    if (edge < 0.15) {
        frag_color = vec4(0.05, 0.05, 0.05, 1.0);  // dark gray edge
        return;
    }

    frag_color = vec4(toon_color + Ia, 1.0);
}