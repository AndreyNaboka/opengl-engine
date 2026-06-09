#version 330 core
in vec3 v_TexCoords;
out vec4 o_Color;

uniform samplerCube u_Skybox;
//uniform float u_Time;   // для динамических эффектов (опционально)

void main() {
    vec4 texColor = texture(u_Skybox, v_TexCoords);
    // Простой эффект мерцания звёзд (на основе яркости)
//    float twinkle = sin(texColor.r * 50.0 + u_Time) * 0.2;
//    texColor.rgb += vec3(twinkle);
    o_Color = texColor;
}
