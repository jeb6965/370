#version 400
out vec4 frag_color;
in vec3 vert_pos, vert_norm;

// light properties
vec3 l_a = vec3(0.2, 0.2, 0.2);
vec3 l_d = vec3(1.0, 1.0, 1.0);
//vec3 l_s = vec3(0.5, 0.0, 0.5);

// material properties
vec3 m_a = vec3 (1.0, 1.0, 1.0);
vec3 m_d = vec3(1.0, 1.0, 1.0);
//vec3 m_s = vec3 (1.0, 1.0, 1.0); 
float specular_component = 10.0;

// light position
vec3 light_pos = vec3(0.0, 1.0, 1.0);

void main() {
    vec3 N = normalize(vert_norm);
    vec3 L = normalize(light_pos - vert_pos);
    vec3 V = normalize(-vert_pos);   

    // ambient
    vec3 Ia = l_a * m_a;

    // diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 Id = l_d * m_d * diff;

    // specular
    //float spec = 0.0;
    //vec3 H = normalize(L + V);
    //spec = pow(max(dot(N, H), 0.0), specular_component);
    //vec3 Is = l_s * m_s * spec;

    //frag_color = vec4(Ia + Id + Is, 1.0);
    frag_color = vec4(Ia + Id, 1.0);
}